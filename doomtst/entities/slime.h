#pragma once
#include "onhit.h"
#include "../game/navigation.h"
#include "../game/collision.h"
#include "../game/rigidbody.h"


#include "../player/player.h"
#include "../items/item.h"
#include "../items/loottable.h"
#include "../game/brain.h"
namespace slimes {
	struct SlimeEdge {
		stn::Option<size_t> jump_height;
		v3::Coord offset;
		navigation::GridCoord apply(const navigation::GridCoord& coord) const {
			return navigation::GridCoord{ .pos = coord.pos + v3::Coord(0,jump_height.unwrap_or(0),0) + offset };
		};
		double cost() const {
			if (jump_height) {
				return 4;
			}
			return jump_height.unwrap_or(1);
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
					geo::RayBox movment(ray, v3::Scale3(1 / 1.2f));

					if (i != 0) {
						//the jump is like an L
						geo::ray jump = geo::ray(current.pos, current.pos + v3::Coord(0, i, 0)).translate(v3::unitv / 2);
						geo::RayBox jump_movment(jump, v3::Scale3(1 / 1.2f));
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
		Mob(timing::WorldClock& timer) :despawn_time(timer) {
			despawn_time.set(60);
		}
		timing::Duration despawn_time;
	};
	struct SlimePathFinder :ecs::component {

		using result_type = navigation::ContextResultType<SlimeNavigator>;
		stn::Option<stn::array<result_type>> path;
		SlimePathFinder(timing::Duration time, ecs::Constrained<core::LocalTransform> follow) :last_fix(time), build_time(time), following(follow) {
	
		}
		timing::Duration build_time;
		timing::Duration last_fix;
		ecs::Constrained<core::LocalTransform> following;
	};
	void SlimePathFinderRecipe(ecs::obj object) {
		object.add_component<SlimePathFinder>(object.world().get_resource<timing::WorldClock>().make_duration(), player::player_for(object.world()));
	}
	struct SlimeNavigation :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<SlimePathFinder, core::LocalTransform,Mob> slimes(world);
			for (auto&& [path, transform,mob] : slimes) {
				if (path.path.map_member(&stn::array<result_type>::non_empty).unwrap_or(false) == true) {
					stn::array<result_type>& current_path = path.path.unwrap();
					result_type current_node = current_path[0];
					v3::Point3 goto_pos = v3::Point3(current_node.result().pos) + v3::Scale3(1 / 2.0f).with_y(transform.transform.scale.y / 2);
					navigation::GridCoord endpoint = current_path.last().result();
					navigation::GridCoord real_endpoint(v3::Coord(path.following.get<core::LocalTransform>().transform.position));
					double apx_real_dist = navigation::GridCoord::apx_distance(current_node.result(), real_endpoint);
					double apx_fake_dist = navigation::GridCoord::apx_distance(real_endpoint, endpoint);

					if (apx_real_dist+2 <= apx_fake_dist) {
						path.last_fix.disable();
					}
					else {

						if (v3::dist(transform.transform.position, goto_pos) < .2f) {
							path.path.unwrap().remove_at(0);
							path.last_fix.set(1.5f);
							if (path.path.unwrap().length()==0) {
								path.last_fix.disable();
							}
						}

					}
				}
				grid::Grid& grid = world.get_resource<grid::Grid>();

				SlimeNavigator navigator{ .world = grid };

				if (path.last_fix.is_inactive() || path.build_time.is_inactive()) {
					double max_follow_distance=20;
					if (math::Transform::distance(transform.transform,path.following.get<core::LocalTransform>().transform)< max_follow_distance) {
					navigation::GridCoord to(grid.get_voxel(path.following.get<core::LocalTransform>().transform.position));
					navigation::GridCoord current(grid.get_voxel(transform.transform.position));
					path.path = navigation::a_star(current, to, navigator);
					mob.despawn_time.set(60);
					path.build_time.set(navigation::GridCoord::apx_distance(to, current) + 3);
					path.last_fix.set(1.5f);
					}
				}

			}
		}
	};

	struct SlimePathFollower :ecs::System {
		void run(ecs::Ecs& world) {

			ecs::View<SlimePathFinder, core::LocalTransform, physics::rigidbody,ecs::Owner,Health::EntityHealth> slimes(world);
			for (auto&& [path, transform, body,object,health] : slimes) {
				stn::Option<result_type& > headed = path.path.and_then([](stn::array<result_type >& edge) {return edge.get(0); });
					if (headed) {
						if (health.damage_delay_timer.remaining().unwrap_or(0)>.7f) {
							continue;
						}
						result_type head = headed.unwrap();
						if (head.move.jump_height != stn::None) {
							if (body.isonground) {
								physics::Implulse jump(v3::Vec3(0, 5+head.move.jump_height.unwrap(), 0));
								body.add_impluse(jump);
								headed.unwrap().current.pos.y += headed.unwrap().move.jump_height.unwrap();
								headed.unwrap().move.jump_height = stn::None;
							}
						}
						else {

							v3::Point3 goto_pos = v3::Point3(head.result().pos) + v3::Scale3(1 / 2.0f);


							v3::Vec3 d = goto_pos - transform.transform.position;
							double dist = (d.length());
							v3::Point3 head(goto_pos.x, transform.transform.position.y, goto_pos.z);
							if (dist > 0.1f) {
								double speed = 15.0f;
								v3::Vec3 v = (d.with_length_less_than(1)) * speed;
								v3::Vec3 force = v - body.velocity;
								force.y = 0;
								force = force.with_length_less_than(1);
								double turn_speed = 15.0f;
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
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element(types.from_name("moss_pack"),1,types,4/30.0f) ,items::loot_element(types.from_name("moss"),1,types) });
		}
	};
	struct SlimeRecipe {
		v3::Point3 pos;
		inline void apply(ecs::obj& slime) const {
			slime.add_component<core::LocalTransform>(pos).transform.scale = v3::unit_scale / 1.3f;
			slime.spawn_child<core::TransformRecipe>(pos);

			slime.apply_recipe(renderer::ModelRecipe{ .path{.mesh = MeshPath("meshes\\cubetest.obj"),.texture{"images\\slimetex.png"}} });
			slime.apply_recipe(items::loot_table_recipe<slime_loot_table>);
			if (random::random()>.9f) {
				slime.get_component<renderer::Model>().color = colors::Color(1,.5,.5,1);
				slime.apply_recipe(Health::HealthSpawner(20));
			}
			else {
				slime.apply_recipe(Health::HealthSpawner(10));
			}
			aabb::DynamicColliderRecipe().apply(slime);
			float dmg = 3;
			slime.add_component<Health::FlashOnHit>();
			slime.add_component<ai::Brain>();
			slime.add_component<Mob>(slime.world().get_resource<timing::WorldClock>());
			slime.add_component<Health::DamageOnHit>(player::player_for(slime.world()), 2, 1);
			slime.apply_recipe(physics::Spawner{ .restitution = .8 });
			slime.apply_recipe(SlimePathFinderRecipe);
		}

	};
	struct SlimeAiPlugin {
		void operator()(Core::App& app) {
			app.insert_plugin(ai::BrainPlugin());
			app.emplace_system<SlimePathFollower>();
			app.emplace_system<SlimeNavigation>();
		}
	};
}