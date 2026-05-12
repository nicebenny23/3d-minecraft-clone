#include "../player/player.h"
#include "../util/random.h"
#include "slime.h"
#include "../world/grid.h"
#include"../world/voxeltraversal.h"
#include "../renderer/blockrender.h"

#include "../player/player.h"
#pragma once 
namespace game {
#define enemytag 1
	inline bool should_despawn(v3::Point3 pos, v3::Point3 player,grid::Grid& world) {
		//todo improve
		return !world.bounds().contains_point(pos)||32<v3::dist(pos, player);

	}
	inline bool spawnable_dist(v3::Point3 pos, v3::Point3 player, grid::Grid& world) {
		return !should_despawn(pos, player,world) && 16 < v3::dist(pos, player);

	}
	struct SpawnTimer :ecs::resource {
		SpawnTimer(timing::WorldClock& clock) :next_spawn(clock) {

		}
		timing::Duration next_spawn;
	};
	struct spawn_mobs :ecs::System {
		bool ensure_light_level(v3::Point3 pos, grid::Grid& grid) {
			return 	grid.get_block(grid.get_voxel(pos)).member(&blocks::block::light_passing_through).unwrap_or(0) < 1;
		}

		stn::Option<geo::Box> find_spawn_location(v3::Vec3 Scl,ecs::Ecs& world) {
			size_t max_checks = 8;
			grid::Grid& grid = world.get_resource<grid::Grid>();
			for (size_t chks = 0; chks < max_checks; chks++) {


				stn::Option<geo::Box> spawn_loc = voxtra::find_ground(unit_scale, grid,50,200);
				if (!spawn_loc) {
					return stn::None;
				}
				v3::Point3 pos = spawn_loc.unwrap().center;
				if (!ensure_light_level(pos, grid)) {
					continue;
				}
				if (!spawnable_dist(pos, player::player_for(world).get_component<core::LocalTransform>().transform.position,grid)) {
					continue;
				}

				double charge= 0;
				ecs::View< slimes::Mob, core::LocalTransform> slimes(world);
				for (auto [slime, transform] : slimes) {
					charge += 1 /(1+v3::dist(transform.transform.position, pos));
				}
				double max_charge =.1f;
				if (charge<max_charge) {
					return spawn_loc;
				}
			}
			return stn::None;
		}
		void run(ecs::Ecs& ecs) {
			timing::Duration& duration=ecs.insert_resource<SpawnTimer>(ecs.get_resource<timing::WorldClock>()).next_spawn;
			double spawn_frequency=.04f;
			size_t total_alive = 0;
			ecs::View< slimes::Mob, core::LocalTransform> slimes(ecs);
			for (auto [slime, transform] : slimes) {

				if (slime.despawn_time.is_inactive()||should_despawn(transform.transform.position, player::player_for(ecs).get_component<core::LocalTransform>().transform.position,ecs.get_resource<grid::Grid>())) {
					slime.owner().destroy();
				}
				total_alive++;
			}
			const size_t max_alive =1;
			if (max_alive <= total_alive) {
				return;
			}
			ecs::View< player::player_tag> players(ecs);
			for (auto [player] : players) {
				{
					if (duration.is_inactive()) {

						stn::Option<geo::Box> spawn_loc = find_spawn_location(unitv,ecs);
						if (!spawn_loc) {
							continue;
						} 
						duration.set(1/ spawn_frequency);
						ecs::spawn(ecs, slimes::SlimeRecipe{ .pos{spawn_loc.unwrap().center} });
					}
				}
			}
		}
	};

	void MobSpawnerPlugin(Core::App& app) {

		app.insert_plugin(slimes::SlimeAiPlugin());
		app.emplace_system< spawn_mobs>();

		app.emplace_system<Health::HitProccesor>();
	}
	// !entityspawner_HPP
}