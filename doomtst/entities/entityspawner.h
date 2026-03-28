#include "../player/player.h"
#include "../util/random.h"
#include "slime.h"
#include "../world/grid.h"
#include"../world/voxeltraversal.h"
#include "../renderer/blockrender.h"

#include "../player/player.h"
#pragma once 
#define enemytag 1
inline bool should_despawn(v3::Point3 pos, v3::Point3 player) {

	return 56< v3::dist(pos, player);

}
inline bool spawnable_dist(v3::Point3 pos, v3::Point3 player) {

	return !should_despawn(pos, player) &&12 < v3::dist(pos, player);

}

struct spawn_mobs :ecs::System {
	bool ensure_light_level(v3::Point3 pos, grid::Grid& grid) {
		float average = 0;

		return 	(average < 8);
	}

	stn::Option<math::Box> find_spawn_location(v3::Vec3 Scl, grid::Grid& grid) {
		size_t max_checks = 40;
		for (size_t chks = 0; chks < max_checks; chks++) {


			stn::Option<Box> spawn_loc = voxtra::findground(unit_scale,grid);
			if (!spawn_loc) {
				return stn::None;
			}
			v3::Point3 pos = spawn_loc.unwrap().center;
			if (!ensure_light_level(pos,grid)) {
				//continue;
			}
			if (!spawnable_dist(pos, player::goblin.get_component<ecs::world_transform>().transform.position)) {
				continue;
			}
			return spawn_loc;
		}
		return stn::None;
	}
	void run(ecs::Ecs& ecs) {
		float spawnthreshold = .01f;
		float randomnum = random::random();
		size_t total_alive = 0;

		ecs::View<ecs::With<slimemove>> slimes(ecs);
		for (auto [slime] : slimes) {

			if (should_despawn(slime.owner().get_component<ecs::world_transform>().transform.position, player::goblin.get_component<ecs::world_transform>().transform.position)) {
				slime.owner().destroy();
			}
			total_alive++;
		}
		const size_t max_alive = 1;
		if (max_alive <= total_alive) {
			return;
		}
		ecs::View<ecs::With<player::player_tag>> players(ecs);
		for (auto [player] : players) {
			{
				if (randomnum < spawnthreshold) {

					stn::Option<Box> spawn_loc = find_spawn_location(unitv,ecs.get_resource<grid::Grid>());
					if (!spawn_loc) {
						continue;
					}
					ecs::spawn(ecs, SlimeRecipe{ .pos{spawn_loc.unwrap().center} });
				}
			}
		}
	}
};

struct MobSpawnerPlugin :Core::Plugin {
	void build(Core::App& app) {
		app.emplace_system< spawn_mobs>();
		app.emplace_system<HitProccesor>();
	}
};
// !entityspawner_HPP
