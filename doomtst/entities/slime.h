#pragma once
#include "onhit.h"
#include "../game/navigation.h"
#include "../game/collision.h"
#include "../game/rigidbody.h"


#include "../player/player.h"
#include "../items/item.h"
#include "../items/loottable.h"
#include "../game/brain.h"
#include "../game/close.h"
namespace slimes {
	struct SlimeEdge {
		stn::Option<size_t> jump_height;
		v3::Coord offset;
		navigation::GridCoord apply(const navigation::GridCoord& coord) const {
			return navigation::GridCoord{ .pos = coord.pos + v3::Coord(0,jump_height.unwrap_or(0),0) + offset };
		};
		double acceptable_distance() {
			return 1.0 + jump_height.unwrap();
		}
		double cost() const {
			return 1.0 + jump_height.unwrap_or(0) * 2+random::random()-.5f;
		}
	};
	struct SlimeNavigator {
		grid::Grid& world;
		using node = navigation::GridCoord;
		using edge = SlimeEdge;
		array<SlimeEdge> moves(const navigation::GridCoord& current) {
			stn::List<v3::Coord, 4> points = { v3::Coord(1,0,0),v3::Coord(-1,0,0),v3::Coord(0,0,1),v3::Coord(0,0,-1) };
			array<SlimeEdge> neighbors;
			stn::Option<block&> block_below_mabye = world.get_block(current.pos - v3::Coord(0, 1, 0));

			if (!block_below_mabye) {
				return neighbors;
			}
			block& block_below = block_below_mabye.unwrap();
			if (!block_below.solid()) {
				neighbors.push(SlimeEdge{ .offset = v3::Coord(0, -1, 0) });
				return neighbors;
			}
			bool can_walk = true;
			if (block_below.bounds().scale != blockscale) {
				can_walk = false;
				return neighbors;
			}
			for (v3::Coord xy_offset : points) {
				for (int i = 0; i < 5; i++) {
					if (i==0&&!can_walk) {
						continue;
					}
					v3::Coord next_pos = xy_offset + current.pos + v3::Coord(0, i, 0);

					geo::ray ray = geo::ray(current.pos, next_pos).translate(v3::unitv / 2);
					geo::RayBox movment(ray, v3::Scale3::from_scale(1 / 1.2f));

					if (i != 0) {
						//the jump is like an L
						geo::ray jump = geo::ray(current.pos, current.pos + v3::Coord(0, i, 0)).translate(v3::unitv / 2);
						geo::RayBox jump_movment(jump, v3::Scale3::from_scale(1 / 1.2f));
						if (voxtra::grid_ray_box_cast(jump_movment, world).is_some()) {
							continue;
						}
						movment.ray.start.y += i;
					}
					if (voxtra::grid_ray_box_cast(movment, world).is_some()) {
						continue;
					}
					stn::Option<size_t> jump_height = stn::None;
					if (i != 0) {
						jump_height = i;
					}

					neighbors.push(SlimeEdge{ .jump_height = jump_height ,.offset{xy_offset} });
					if (i != 0) {
						break;
					}
				}
			}
			return neighbors;
		}
	};

	using result_type = navigation::ContextResultType<SlimeNavigator>;

	struct Mob :ecs::component {
	};
	struct SlimePathFinder :ecs::component {

		using result_type = navigation::ContextResultType<SlimeNavigator>;
		stn::array<result_type> path;
		SlimePathFinder(timing::Duration time, ecs::Constrained<core::LocalTransform> follow,double speed) :last_fix(time), build_time(time), following(follow), speed(speed),last_detection(time.clock()){
		}
		double speed;
		//stops path stales
		timing::Duration build_time;
		//last time noticble progress was made in the path
		timing::Duration last_fix;
		//tick for path check
		timing::Duration last_detection;
		stn::Option<v3::Vec3> last_position;
		ecs::Constrained<core::LocalTransform> following;
		void reset_fix() {
			last_fix.set(.5f);
		}
		void force_repath() {
			last_fix.disable();
		}
	};
	inline v3::Point3 slime_target(ecs::Constrained<SlimePathFinder, core::LocalTransform> finder) {
		SlimePathFinder& path = finder.get<SlimePathFinder>();
		v3::Point3 pnt = path.following.get_component<core::LocalTransform>().transform.position;
		return pnt;
	}
	struct Idler{
	};

	struct SlimeNavigation :ecs::System {
		void run(ecs::Ecs& world) {
			
			ecs::View<SlimePathFinder, core::LocalTransform, Mob, ecs::Owner, ai::Brain> slimes(world);
			for (auto&& [path, transform, mob, object, brain] : slimes) {

				double max_follow_distance = 26;
				if (v3::dist(transform.transform.position, slime_target(object)) >= max_follow_distance) {
					brain.set<Idler>(1);
				}
				if (!brain.active<SlimeNavigator>()) {
					continue;
				}
				if (path.last_detection.is_inactive_set(.1)) {
					double min_dist = .08f;
					//if their is no last position or we move to much we reset fix otherwise fix eventually becomes innactive
					if (!path.last_position || v3::dist(path.last_position.unwrap(), transform.transform.position) >= min_dist) {
						path.reset_fix();
					}
					path.last_position = transform.transform.position;
				}
				v3::Vec3 off = transform.transform.position - path.following.get<core::LocalTransform>().transform.position;
				if (path.path.non_empty()) {
					result_type current_node=path.path.first();
					v3::Point3 goto_pos = v3::Point3(current_node.result().pos) + v3::Scale3::from_scale(1 / 2.0f).with_y(transform.transform.scale.y / 2);
					navigation::GridCoord endpoint = path.path.last().result();
					navigation::GridCoord real_endpoint(v3::Coord::from_vec3(slime_target(object)));
					double apx_real_dist = navigation::GridCoord::apx_distance(current_node.result(), real_endpoint);
					double apx_fake_dist = navigation::GridCoord::apx_distance(real_endpoint, endpoint);
					if (apx_real_dist + 1 <= apx_fake_dist) {
						path.force_repath();
					}
					else {

						if (v3::dist(transform.transform.position, goto_pos) < .2f) {
							path.path.remove_at(0);
							if (path.path.empty()) {
								path.force_repath();
							}
						}

					}
				}
				grid::Grid& grid = world.get_resource<grid::Grid>();

				SlimeNavigator navigator{ .world = grid };

				if (path.last_fix.is_inactive() || path.build_time.is_inactive()) {
					navigation::GridCoord to(grid.get_voxel(slime_target(object)));
					navigation::GridCoord current(grid.get_voxel(transform.transform.position));
					path.path = navigation::a_star(current, to, navigator).unwrap_or_default();
					path.build_time.set(navigation::GridCoord::apx_distance(to, current) + 2);
					path.reset_fix();
				}
			}

		}
	};
	struct SlimeStunner :ecs::System {

		void run(ecs::Ecs& world) {
			ecs::View < SlimePathFinder, ai::Brain, Health::EntityHealth > slimes(world);
				for (auto&& [path, brain, health] : slimes) {
					if (Health::damage_delay-.2f < health.damage_delay_timer.remaining().unwrap_or(0)) {
						brain.set<Idler>(1);
					}
				}
		}



	};
	struct SlimePathFollower :ecs::System {
		void run(ecs::Ecs& world) {
			if (!player::in_game(world)) {
				return;
			}
			ecs::View<SlimePathFinder, core::LocalTransform, physics::RigidBody,ecs::Owner,Health::EntityHealth,ai::Brain> slimes(world);
			for (auto&& [path, transform, body,object,health,brain] : slimes) {
				brain.set<SlimeNavigator>(0);
				if (!brain.active<SlimeNavigator>()) {
					continue;
				}
				stn::Option<result_type& > headed = path.path.first_opt();
					if (headed) {
						result_type head = headed.unwrap();
						if (head.move.jump_height != stn::None) {
							if (body.on_ground) {
								physics::Implulse jump(v3::Vec3(0, 3.5+head.move.jump_height.unwrap(), 0));
								body.add_impluse(jump);
								headed.unwrap().current.pos.y += headed.unwrap().move.jump_height.unwrap();
								headed.unwrap().move.jump_height = stn::None;
							}
						}
						else {
						
							v3::Point3 goto_pos = v3::Point3(head.result().pos) + v3::Scale3::from_scale(1 / 2.0f);


 							v3::Vec3 d = goto_pos - transform.transform.position;
							d.y = 0;
							double dist = (d.length());
						
							v3::Point3 head(goto_pos.x, transform.transform.position.y, goto_pos.z);
							if (dist > 0.1f) {
								double speed = path.speed;
								v3::Vec3 v = (d.with_length_less_than(1)) * speed;
								v3::Vec3 force = v - body.velocity;
								force.y = 0;
								force = force.with_length_less_than(1);
								double turn_speed = speed;
								body.add_force(physics::Force(force * turn_speed));
								v3::Vec3 look = v3::lerp(transform.transform.normal_dir(), (head - transform.transform.position), world.get_resource<timing::WorldClock>().dt * 10);
								transform.transform.look_towards(look);
							}

						}
					}
			}
		}
	};
	struct slime_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element(types.from_name("moss_pack"),1,types,4/30.0f) ,items::loot_element(types.from_name("moss"),1,types) });
		}
	};
	struct blue_slime_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element(types.from_name("moss_pack"),1,types,6 / 30.0f) ,items::loot_element(types.from_name("moss"),2,types) });
		}
	};
	struct SlimeRecipe {
		v3::Point3 pos;
		inline void apply(ecs::obj& slime) const {
			slime.add_component<core::LocalTransform>(pos).transform.scale = v3::unit_scale / 1.3f;
			slime.spawn_child_emplaced<core::TransformRecipe>(pos);

			double speed = 15;
			if (random::random()>.9f) {

				slime.apply_recipe(items::loot_table_recipe<blue_slime_loot_table>);
				slime.apply_recipe(renderer::ModelRecipe{ .path{.mesh = MeshPath("meshes\\cubetest.obj"),.texture{"images\\slimetexblue.png"}} });
				slime.apply_recipe(Health::HealthSpawner(20));
				speed = 20;
			}
			else {

				slime.apply_recipe(items::loot_table_recipe<slime_loot_table>);
				slime.apply_recipe(renderer::ModelRecipe{ .path{.mesh = MeshPath("meshes\\cubetest.obj"),.texture{"images\\slimetex.png"}} });
				slime.apply_recipe(Health::HealthSpawner(10));
			}
			aabb::DynamicColliderRecipe().apply(slime);
			float dmg = 3; 
			slime.add_component<SlimePathFinder>(slime.world().get_resource<timing::WorldClock>().make_duration(), player::player_for(slime.world()),speed);
			slime.add_component<Health::FlashOnHit>();
			slime.add_component<Mob>();
			slime.add_component<ai::Brain>();
			slime.add_component<Health::DamageOnHit>(player::player_for(slime.world()), 2, 5);
			slime.apply_recipe(physics::Spawner{ .restitution = .8 });
		}

	};
	struct SlimeAiPlugin {
		void operator()(core::App& app) {
			app.insert_plugin(ai::BrainPlugin());
			app.emplace_system< SlimeStunner>();
			app.emplace_system<SlimePathFollower>();
			app.emplace_system<SlimeNavigation>();
		}
	};
}