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
			return 1.0 + jump_height.unwrap_or(0) * 2+random::random()/4.0f;
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
			stn::Option<chunks::block_object&> mabye_at = world.get_object(current.pos);

			if (!block_below_mabye|| !mabye_at) {
				return neighbors;
			}
			block& block_below = block_below_mabye.unwrap();
			chunks::block_object& block_at = mabye_at.unwrap();
			bool liquid = block_at.has_component<Liquid>();
			//gas
			bool water_float = block_below.is<WaterBlock>() && !liquid;
			if (!block_below.solid()&&!block_below.is<WaterBlock>() && !liquid) {
				neighbors.push(SlimeEdge{ .offset = v3::Coord(0, -1, 0) });
				return neighbors;
			}
			//liquid
			if (liquid) {
				for (math::Direction3d offset : math::Directions3d) {
					if (water_float&&offset==math::up_3d) {
						continue;
					}
					v3::Coord next_pos = offset.coord() + current.pos;
					geo::ray ray = geo::ray(current.pos, next_pos).translate(v3::unitv / 2);
					geo::RayBox movment(ray, v3::Scale3::from_scale(1 / 1.2f));
					if (voxtra::grid_ray_box_cast<SolidPredicate>(movment, world).is_some()) {
						continue;

					}

					neighbors.push(SlimeEdge{ .jump_height=stn::None,.offset=offset.coord() });
				}
				return neighbors;	
			}
			//solid
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
						if (voxtra::grid_ray_box_cast<SolidPredicate>(jump_movment, world).is_some()) {
							continue;
						}
						movment.ray.start.y += i;
					}
					if (voxtra::grid_ray_box_cast<SolidPredicate>(movment, world).is_some()) {
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

	//enemies all pathfind
	struct Enemy :ecs::component {
		Enemy(timing::Clock& clock):last_fix(clock),last_detection(clock),build_time(clock){

		}
		math::bounds pursue_range=math::bounds(0,35);
		double look_distance=18;
		timing::Duration last_fix;
		//tick for path check
		timing::Duration last_detection;
		stn::Option<v3::Vec3> last_position;

		//stops path stales
		timing::Duration build_time;
		void reset_fix() {
			last_fix.set(.5f);
		}
		void force_repath() {
			last_fix.disable();
		}
	};

	struct SlimePathFinder :ecs::component {

		using result_type = navigation::ContextResultType<SlimeNavigator>;
		stn::array<result_type> path;
		SlimePathFinder(ecs::Constrained<core::LocalTransform> follow,double speed,v3::Vec3 spawn_coords) :following(follow), speed(speed), spawn(spawn_coords){
		}
		double speed;
		v3::Vec3 spawn;
		//last time noticble progress was made in the path
		ecs::Constrained<core::LocalTransform> following;
		stn::Option<v3::Vec3> random_walk_position;
		
	};

	struct Idler {
	};
	struct Wandering {

	};

	inline v3::Point3 slime_target(ecs::Constrained<SlimePathFinder, core::LocalTransform,ai::Brain> finder) {
		SlimePathFinder& path = finder.get<SlimePathFinder>();
		v3::Point3 pnt = path.following.get_component<core::LocalTransform>().transform.position;

		if (finder.get<ai::Brain>().active<Wandering>()) {
			math::Transform us = finder.get<core::LocalTransform>().transform;
			if (path.random_walk_position&&v3::dist(us.position, path.random_walk_position.unwrap())<=1) {
				path.random_walk_position = stn::None;
			}

			if (!finder.world().get_resource<grid::Grid>().contains_chunk(chunks::ChunkLocation::from_block_pos(v3::Coord::from_vec3(path.spawn)))) {
				path.random_walk_position= us.position;
				path.spawn = us.position;
			}
			while(!path.random_walk_position) {	
				v3::Vec3 random = random::spherical();
				random *= 15* random::random();
				path.random_walk_position=voxtra::find_ground_at(us.unrotated_box().with_center(path.spawn).translated(random), finder.world().get_resource<grid::Grid>()).member(&geo::Box::center);
			}

			return path.random_walk_position.unwrap();
		}
		return pnt;
	}

	struct SlimeNavigation :ecs::System {
		void run(ecs::Ecs& world) {
			
			ecs::View<SlimePathFinder, core::LocalTransform, Enemy, ecs::Owner, ai::Brain> slimes(world);
			for (auto [path, transform, mob, object, brain] : slimes) {
				double dist = v3::dist(transform.transform.position, path.following.get_component<core::LocalTransform>().transform.position);
				double home_dist = v3::dist(path.spawn, path.following.get_component<core::LocalTransform>().transform.position);
				double self_home_dist = v3::dist(path.spawn, transform.transform.position);
				brain.allow<Wandering>();
				if (brain.becoming_active<SlimeNavigator>()|| brain.becoming_active<Wandering>()) {
					mob.force_repath();
				}
				if (mob.pursue_range.contains(home_dist)&&(brain.active<SlimeNavigator>()|| dist < mob.look_distance)) {
					brain.allow<SlimeNavigator>();
				}
				
				if (!brain.active<SlimeNavigator>()&&!brain.active<Wandering>()) {
					continue;
				}
				if (mob.last_detection.is_inactive_set(.1)) {
					double min_dist = .08f;
					//if their is no last position or we move to much we reset fix otherwise fix eventually becomes innactive
					if (!mob.last_position || v3::dist(mob.last_position.unwrap(), transform.transform.position) >= min_dist) {
						mob.reset_fix();
					}
					mob.last_position = transform.transform.position;
				}

				if (path.path.non_empty()) {
					result_type current_node=path.path.first();
					v3::Point3 goto_pos = v3::Point3(current_node.result().pos) + v3::Scale3::from_scale(1 / 2.0f).with_y(transform.transform.scale.y / 2);
					navigation::GridCoord endpoint = path.path.last().result();
					navigation::GridCoord real_endpoint(v3::Coord::from_vec3(slime_target(object)));
					double apx_real_dist = navigation::GridCoord::apx_distance(current_node.result(), real_endpoint);
					double apx_fake_dist = navigation::GridCoord::apx_distance(real_endpoint, endpoint);
					if (apx_real_dist + 1 <= apx_fake_dist) {
						mob.force_repath();
					}
					else {

						if (v3::dist(transform.transform.position, goto_pos) < .2f) {
							path.path.remove_at(0);
							if (path.path.empty()) {
								mob.force_repath();
							}
						}

					}
				}
				grid::Grid& grid = world.get_resource<grid::Grid>();

				SlimeNavigator navigator{ .world = grid };

				if (mob.last_fix.is_inactive() || mob.build_time.is_inactive()) {
					navigation::GridCoord to(grid.get_voxel(slime_target(object)));
					navigation::GridCoord current(grid.get_voxel(transform.transform.position));
					path.path = navigation::a_star(current, to, navigator,1000).unwrap_or_default();
					mob.build_time.set(navigation::GridCoord::apx_distance(to, current) + 2);
					mob.reset_fix();
					if (path.path.empty()) {
						path.random_walk_position = stn::None;
					}
				}
			}

		}
	};
	struct SlimeStunner :ecs::System {

		void run(ecs::Ecs& world) {
			ecs::View < SlimePathFinder, ai::Brain, Health::EntityHealth > slimes(world);
				for (auto&& [path, brain, health] : slimes) {
					if (Health::damage_delay-.2f < health.damage_delay_timer.remaining().unwrap_or(0)) {
						brain.allow<Idler>();
					}
				}
		}



	};
	struct SlimePathFollower :ecs::System {
		void run(ecs::Ecs& world) {
			if (!player::in_game(world)) {
				return;
			}
			ecs::View<SlimePathFinder, core::LocalTransform, physics::RigidBody,ecs::Owner,Health::EntityHealth,ai::Brain, physics::Buoyancy> slimes(world);
			for (auto&& [path, transform, body,object,health,brain,buoyancy] : slimes) {
				if (!brain.active<SlimeNavigator>()&&!brain.active< Wandering>()) {
					continue;
				}
				stn::Option<result_type& > headed = path.path.first_opt();
					if (headed) {
						result_type head = headed.unwrap();
						if (head.move.jump_height != stn::None) {
							if (body.on_ground) {
								body.velocity+=v3::Vec3(0,7+2.2*head.move.jump_height.unwrap(),0);
								headed.unwrap().current.pos.y += headed.unwrap().move.jump_height.unwrap();
								headed.unwrap().move.jump_height = stn::None;
							}
						}
						else {
						
							v3::Point3 goto_pos = v3::Point3(head.result().pos) + v3::Scale3::from_scale(1 / 2.0f);


 							v3::Vec3 d = goto_pos - transform.transform.position;
							if (!buoyancy.in_water) {
								d.y = 0;
							}
							double dist = (d.length());
							v3::Point3 headed(goto_pos.x, transform.transform.position.y, goto_pos.z);
							if (dist > 0.1f) {
								double speed = path.speed;
								v3::Vec3 v = (d.with_length_less_than(1)) * speed;
								v3::Vec3 force = v - body.velocity;
								if (!buoyancy.in_water) {
									force.y = 0;
								}
								force = force.with_length_less_than(1);
								if (buoyancy.in_water) {
									//for now
									//cheat for now
									body.add_acceleration(v3::Vec3(0, 25.53 * math::sign_rounding_up(d.y), 0));
									double vel = body.velocity.y;
								}

								double turn_speed = speed;
								body.add_force(force * turn_speed);
								v3::Vec3 look = head.move.offset;
								look.y = 0;
								if (look.mag2()>=.5f) {

									transform.transform.look=math::rotate_twords(transform.transform.look,look.look(), world.get_resource<timing::GameClock>().game_clock.dt * glm::two_pi<double>()*2);
								}
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

			double speed = 14;
			if (random::random()>.9f) {

				slime.apply_recipe(items::loot_table_recipe<blue_slime_loot_table>);
				slime.apply_recipe(renderer::ModelRecipe{ .path{.mesh = MeshPath("meshes\\cubetest.obj"),.texture{"images\\slimetexblue.png"}} });
				slime.apply_recipe(Health::HealthSpawner(20));
				speed =18;
			}
			else {

				slime.apply_recipe(items::loot_table_recipe<slime_loot_table>);
				slime.apply_recipe(renderer::ModelRecipe{ .path{.mesh = MeshPath("meshes\\cubetest.obj"),.texture{"images\\slimetex.png"}} });
				slime.apply_recipe(Health::HealthSpawner(10));
			}
			collision::DynamicColliderRecipe().apply(slime);
			float dmg = 3; 
			slime.add_component<SlimePathFinder>(player::player_for(slime.world()),speed,pos);
			slime.add_component<Health::FlashOnHit>();
			slime.add_component<Enemy>(slime.world().get_resource<timing::GameClock>().game_clock);
			ai::Brain& brain=slime.add_component<ai::Brain>();
			brain.add_behavior<Wandering>(-1);
			brain.add_behavior<SlimeNavigator>(0);
			brain.add_behavior<Idler>(1);
			slime.add_component<Health::DamageOnHit>(player::player_for(slime.world()), 2,2);
			slime.apply_recipe(physics::Spawner{ .restitution =1.0,.density = 1.0,.gravity=v3::Vec3(0,-25.53,0)});
		}
  
	};
	struct SlimeAiPlugin {
		void operator()(core::App& app) {
			app.insert_plugin(ai::BrainPlugin());
			app.emplace_system< SlimeStunner>();
			app.emplace_system<SlimeNavigation>();
			app.emplace_system<SlimePathFollower>();
		}
	};
}