#pragma once
#include "onhit.h"
#include "../game/navigation.h"
#include "../game/collision.h"
#include "../game/rigidbody.h"


#include "../player/player.h"
#include "../items/item.h"
#include "../items/loottable.h"

struct SlimeNavigator {
	grid::Grid& world;
	using node = navigation::GridCoord;
	array<navigation::WithCost<navigation::GridCoord>> neighbors(navigation::GridCoord& coord) {
		array<navigation::WithCost<navigation::GridCoord>> neighbors;
		for (int xind = -1; xind <= 1; xind++) {
			for (int yind = -1; yind <= 1; yind++) {


				for (int zind = -1; zind <= 1; zind++) {
					Coord offset = Coord(xind, yind, zind);

					//new l0cation
					v3::Coord place = offset + coord.pos;
					if (offset.x != 0 && offset.z != 0) {
						continue;
					}
					//if same
					if (place == coord.pos) {
						continue;
					}
					//center of block
					v3::Point3 center = place + unitv / 2;

					v3::Scale3 scale = blockscale * .99f;
					//where we are now
					math::Box bx = math::Box(coord.pos + v3::Vec3(1 / 2.f, 1 / 2.f, 1 / 2.f), scale);

					bool cango = true;
					//if close enigh		
					if (yind != -1) {
						Coord testblock = place - Coord(0, 1, 0);
						if (world.get_block(testblock + coord.pos).is_none()) {
							continue;
						}
					}
					for (int i = 0; i < 3; i++) {
						bx.center += Vec3(offset[i], i);


						if (voxtra::boxcast_grid(bx, world)) {
							cango = false;
						}
					}

					if (cango) {
						neighbors.push(navigation::WithCost(navigation::GridCoord{ place }, 1));
					}
				}


			}
		}
		return neighbors;
	}
};

struct slimemove : ecs::component {
	bool ctype;
	float timesincejump;
	void jump() {
		owner().get_component<physics::rigidbody>().velocity.y = 9;
	}
	void start() {

		timesincejump = 1;
	}
	void update() {
		timesincejump -= world().ensure_resource<timename::TimeManager>().dt;
		Point3 pos = owner().get_component<ecs::world_transform>().transform.position;
		navigation::NavigationComponent& comp = owner().get_component<navigation::NavigationComponent>();
		if (random::random()<.01f) {
			comp.locations = navigation::a_star(navigation::GridCoord(v3::Coord(pos)), navigation::GridCoord(v3::Coord(comp.following.get<ecs::world_transform>().transform.position)), SlimeNavigator{ .world = comp.world().get_resource<grid::Grid>() })
				.unwrap_or_default();

		}
		v3::Coord headed = comp.locations.get(1).copied().unwrap_or(navigation::GridCoord(v3::Coord(pos))).pos;
		v3::Vec3 gotopos = v3::Point3(headed) - pos;
		if (gotopos != zerov) {

			if (gotopos.y > .6f) {
				if (timesincejump < 0) {

					timesincejump = 3;
					jump();
				}
			}


			owner().get_component<physics::rigidbody>().add_force(physics::Force(v3::Vec3(normal(gotopos).x, 0, normal(gotopos).z)));
			owner().get_component<ecs::world_transform>().transform.look_towards(v3::Vec3(gotopos.x, 0, gotopos.z));


		}
	}
};
struct slime_loot_table :items::LootTable {
	items::LootDrops drops_for(items::item_types& types) {
		return items::LootDrops({ items::loot_element(types.from_name("moss"),1,types)});
	}
};
struct SlimeRecipe {
	v3::Coord pos;
	inline void apply(ecs::obj& slime) const {
		slime.add_component<ecs::world_transform>(pos);

		slime.apply_recipe(renderer::ModelRecipe{ .path{.mesh = "meshes\\cubetest.obj",.texture{"images\\slimetex.png"}} });
		slime.add_component<navigation::NavigationComponent>(ecs::Constrained<ecs::world_transform>(player::player_for(slime.world())));
		slime.apply_recipe(items::loot_table_recipe<slime_loot_table>);
		slime.apply_recipe(Health::HealthSpawner(10));
		aabb::DynamicColliderRecipe().apply(slime);
		float dmg = 3;

		slime.add_component<DamageOnHit>(dmg, dmg * 2);
		slime.apply_recipe(physics::Spawner);
		slime.add_component<slimemove>();
		slime.get_component<ecs::world_transform>().transform.scale = blockscale;
	}

};
