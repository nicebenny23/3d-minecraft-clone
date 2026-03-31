#pragma once
#include "onhit.h"
#include "../game/navigation.h"
#include "../game/collision.h"
#include "../game/rigidbody.h"


#include "../player/player.h"
#include "../items/item.h"
#include "../items/loottable.h"
namespace slimes {
	struct SlimeEdge {
		stn::Option<size_t> jump_height;
		v3::Coord offset;
		navigation::GridCoord apply(const navigation::GridCoord& coord) const {
			return navigation::GridCoord{ .pos = coord.pos + v3::Coord(0,jump_height.unwrap_or(0),0) + offset };
		};
		double cost() const {
			if (jump_height) {
				return 3;
			}
			return jump_height.unwrap_or(1);
		}
		v3::Vec3 movment() {
			return v3::Vec3(offset.x, jump_height.unwrap_or(0), offset.z);
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
			neighbors.reserve(8);
			for (v3::Coord xy_offset : points) {
				for (int i = 0; i < 2; i++) {
					v3::Coord next_pos = xy_offset + current.pos + v3::Coord(0, i, 0);
					stn::Option<block&> blk = world.get_block(next_pos);
					if (!blk || blk.unwrap().solid()) {
						continue;
					}
					//new l0cation
					geo::ray ray = geo::ray(current.pos, next_pos).translate(v3::unitv / 2);

					//center of block
					geo::RayBox movment(ray, v3::Scale3(1 / 1.2f));
					v3::Scale3 scale = blockscale * .99f;
					//where we are now
					bool cango = true;
					//if close enigh		
					if (i == 1) {
						movment.start_box().center += v3::Vec3(0, 1, 0);
					}


					stn::Option<size_t> jump_height = stn::None;
					if (i == 1) {
						jump_height = i;
					}
					neighbors.push(SlimeEdge{ .jump_height = jump_height ,.offset{xy_offset} });
				}
			}
			return neighbors;
		}
	};

	using result_type = navigation::ContextResultType<SlimeNavigator>;

	struct SlimePathFinder :ecs::component {

		using result_type = navigation::ContextResultType<SlimeNavigator>;
		stn::Option<stn::array<result_type>> path;
		SlimePathFinder(timename::Duration time,ecs::Constrained<ecs::world_transform> follow) :last_fix(time),build_time(time), following(follow) {

		}

		timename::Duration build_time;
		timename::Duration last_fix;
		ecs::Constrained<ecs::world_transform> following;
	};
	void SlimePathFinderRecipe(ecs::obj object) {
		object.add_component<SlimePathFinder>(object.world().get_resource<timename::TimeManager>().current_time(), player::player_for(object.world()));
	}
	struct SlimeNavigation :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<SlimePathFinder, ecs::world_transform> slimes(world);
			for (auto&& [path, transform] : slimes) {
				if (path.path.map_member(&stn::array<result_type>::non_empty).unwrap_or(false) == true) {
					result_type current_node = path.path.unwrap()[0];
					v3::Point3 goto_pos = v3::Point3(current_node.result().pos) + v3::Scale3(1 / 2.0f).with_y(transform.transform.scale.y/2);

					if (v3::dist(transform.transform.position, goto_pos) < .1f) {
						path.path.unwrap().remove_at(0);
						path.last_fix.set(2.0f);
					}
				}
				grid::Grid& grid = world.get_resource<grid::Grid>();

				SlimeNavigator navigator{ .world = grid };
				if (path.last_fix.is_inactive()||path.build_time.is_inactive()) {

					navigation::GridCoord to(grid.get_voxel(path.following.get<ecs::world_transform>().transform.position));
					navigation::GridCoord current(grid.get_voxel(transform.transform.position));
					path.path = navigation::a_star(current, to, navigator);
					path.last_fix.set(2.0f);
					path.build_time.set(10.0f);
				}

			}
		}
	};
	struct SlimePathFollower :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<SlimePathFinder, ecs::world_transform, physics::rigidbody> slimes(world);
			for (auto&& [path, transform, body] : slimes) {
				stn::Option<result_type& > headed = path.path.and_then([](stn::array<result_type >& edge) {return edge.get(0); });

				if (headed) {
					result_type head = headed.unwrap();

					if (head.move.jump_height != stn::None) {
						physics::Implulse jump(v3::Vec3(0, head.move.jump_height.unwrap() * 10, 0));
						body.velocity.y = (jump.impulse.y);
						headed.unwrap().move.jump_height = stn::None;
						headed.unwrap().current.pos.y += 1;
					}
					else {

						v3::Point3 goto_pos = v3::Point3(head.result().pos) + v3::Scale3(1 / 2.0f);


						v3::Vec3 d = goto_pos - transform.transform.position;
						double dist = (d.length());
						v3::Point3 head(goto_pos.x, transform.transform.position.y, goto_pos.z);
						if (dist > 0.001f) {
							double speed = 5.0f;
							v3::Vec3 v = (d.with_length_less_than(1)) * speed;
							v3::Vec3 force = v - body.velocity;
							force.y = 0;
							force = force.with_length_less_than(1);
							double turn_speed = 5.0f;
							body.add_force(physics::Force(force * turn_speed));
							transform.transform.look_at(head);
						}

					}
				}
			}
		}


	};
	struct slime_loot_table :items::LootTable {
		items::LootDrops drops_for(items::item_types& types) {
			return items::LootDrops({ items::loot_element(types.from_name("moss"),1,types) });
		}
	};
	struct SlimeRecipe {
		v3::Coord pos;
		inline void apply(ecs::obj& slime) const {
			slime.add_component<ecs::world_transform>(pos).transform.scale = v3::unit_scale / 1.3f;

			slime.apply_recipe(renderer::ModelRecipe{ .path{.mesh = "meshes\\cubetest.obj",.texture{"images\\slimetex.png"}} });
			slime.apply_recipe(items::loot_table_recipe<slime_loot_table>);
			slime.apply_recipe(Health::HealthSpawner(10));
			aabb::DynamicColliderRecipe().apply(slime);
			float dmg = 3;
			slime.add_component<Health::FlashOnHit>();
			slime.add_component<Health::DamageOnHit>(dmg, dmg * 2);
			slime.apply_recipe(physics::Spawner);
			slime.apply_recipe(SlimePathFinderRecipe);
			slime.get_component<ecs::world_transform>().transform.scale = blockscale;
		}

	};
	struct SlimeAiPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system<SlimePathFollower>();
			app.emplace_system<SlimeNavigation>();
		}
	};
}