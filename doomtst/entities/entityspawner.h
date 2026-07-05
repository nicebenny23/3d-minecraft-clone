#include "../player/player.h"
#include "../util/random.h"
#include "slime.h"
#include "../world/grid.h"
#include"../world/voxeltraversal.h"
#include "../renderer/blockrender.h"

#include "../player/player.h"
#pragma once 
namespace game {
	inline bool should_despawn(v3::Point3 pos, v3::Point3 player,grid::Grid& world) {
		//todo improve
		return !world.bounds().contains_point(pos)||40<v3::dist(pos, player);

	}
	inline bool spawnable_dist(v3::Point3 pos, v3::Point3 player, grid::Grid& world) {
		return !should_despawn(pos, player,world) && 24 < v3::dist(pos, player);

	}


	inline bool ensure_light_level(v3::Point3 pos, grid::Grid& grid) {
		return 	grid.get_block(grid.get_voxel(pos)).member(&blocks::block::light_passing_through).unwrap_or(0) < 1;
	}
	struct EntitySpawn {
		int count=2;
		EntitySpawn(size_t entities = 1):count(entities){

		}
		bool try_spawn_many(size_t count, ecs::Ecs& world) {

			for (size_t i = 0; i < count; i++) {
				if (try_spawn_once(world)) {
					return true;
				}
			}
			return false;
		}
		bool try_spawn_once(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			v3::Scale3 scale = unit_scale.with_x(count);
			stn::Option<geo::Box> spawn_loc = voxtra::find_ground(unit_scale, grid, 50, 200);
			if (!spawn_loc) {
				return false;
			}
			v3::Point3 pos = spawn_loc.unwrap().center;
			if (!ensure_light_level(pos, grid)) {
				return false;
			}
			if (!spawnable_dist(pos, player::player_for(world).get_component<core::LocalTransform>().transform.position, grid)) {
				return false;
			}
			double charge = 0;
			ecs::View< slimes::Mob, core::LocalTransform> slimes(world);
			for (auto [slime, transform] : slimes) {
				charge += 1 / (1 + v3::dist(transform.transform.position, pos));
			}
			double max_charge = .10f;
			if (max_charge<=charge) {
				return false;
			}

			for (int i = 0; i < count; i++) {
				v3::Point3 spawn_pos = pos + v3::Vec3(i, 0, 0);

				ecs::spawn(world, slimes::SlimeRecipe{ .pos{spawn_pos} });

			}
			return true;
			
		}
	};
	struct SpawnTimer :ecs::resource {
		SpawnTimer(timing::WorldClock& clock) :next_spawn(clock) {

		}
		EntitySpawn next;
		timing::Duration next_spawn;
	};
	struct spawn_mobs :ecs::System {
		
		
		void run(ecs::Ecs& ecs) {
			SpawnTimer& spawn_timer= ecs.insert_resource<SpawnTimer>(ecs.get_resource<timing::WorldClock>());
			timing::Duration& duration= spawn_timer.next_spawn;
			double spawn_frequency=.2f;
			size_t total_alive = 0;
			ecs::View< slimes::Mob, core::LocalTransform> slimes(ecs);
			for (auto [slime, transform] : slimes) {

				if (should_despawn(transform.transform.position, player::player_for(ecs).get_component<core::LocalTransform>().transform.position,ecs.get_resource<grid::Grid>())) {
					slime.owner().destroy();
				}
				total_alive++;
			}
			const size_t max_alive =40;
			if (max_alive <= total_alive) {
				return;
			}
			ecs::View< player::PlayerTag> players(ecs);
			for (auto [player] : players) {
				{
					if (duration.is_inactive()) {

						if (!spawn_timer.next.try_spawn_many(10,ecs)) {
							continue;
						} 
						int count = int(3*math::ease_in_power(random::random(),5))+1;
						spawn_timer.next = EntitySpawn(count);
						duration.set(1/ spawn_frequency);
					}
				}
			}
		}
	};

	void MobSpawnerPlugin(core::App& app) {

		app.insert_plugin(slimes::SlimeAiPlugin());
		app.emplace_system< spawn_mobs>();

		app.emplace_system<Health::HitProccesor>();
	}
	// !entityspawner_HPP
}