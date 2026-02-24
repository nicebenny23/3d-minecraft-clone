#pragma once
#include "../game/ecs/multi_query.h"
#include "grid.h"
#include "../block/air.h"

#include "../util/mutex.h"
#include "../util/cached.h"
#include <cstdint>
#include <tuple>
#include <utility>
#include "../block/block.h"
#include "../game/ecs/ecs.h"
#include "../game/ecs/game_object.h"
#include "../game/ecs/query.h"
#include "../game/ecs/system.h"
#include "../math/dir.h"
#include "../util/Option.h"
#include "../util/queue.h"
#include "../math/vector3.h"
#include "../util/reference.h"
namespace grid {
	struct partial_darken_command {
		v3::Coord location;
		size_t old_light;
	};
	struct lighten_block_command {
		v3::Coord location;
	};
	struct shade_at {
		shade_at(blocks::block& block_for, size_t light_value) :block(block_for), light(light_value) {
		}
		stn::non_null<blocks::block> block;
		size_t light;
	};
	struct light_changed {
		v3::Coord location;
	};
	struct LightRemover :ecs::System {
		LightRemover(ecs::Ecs& world) {

		}
		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			stn::queue<shade_at> darkening_queue;
			for (partial_darken_command& cmd : world.read_commands<partial_darken_command>()) {
				grid
					.get_block(cmd.location)
					.then([&](blocks::block& blk) {darkening_queue.push(shade_at(blk, cmd.old_light)); });

			}
			while (!darkening_queue.empty()) {
				shade_at shade = darkening_queue.pop();
				//if the queue continued while being zero then we would run forever
				if (shade.light == 0) {
					continue;
				}
				for (auto dir : math::Directions3d) {
					Coord look_position = shade.block->pos + dir.coord();
					grid.get_block(look_position).then([&](block& next_block) {
						if (next_block.mesh.is_transparent()) {
							if (shade.light <= next_block.light_passing_through) {
								world.write_command(lighten_block_command({ next_block.pos }));
							}
							else {
								darkening_queue.push(shade_at(next_block, next_block.light_passing_through));
								next_block.light_passing_through = 0;
							}
						}
						if (!next_block.mesh.invisible()) {
							next_block[dir.inverse_index()].set_light(0);
						}
					});
				}
			}
		}
	};


	struct GridLighter :ecs::System {
		ecs::EventReader<Chunk::chunk_loaded> chunk_loads;
		GridLighter(ecs::Ecs& world) :chunk_loads(world.make_reader<Chunk::chunk_loaded>()) {
		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			stn::queue<stn::non_null<block>> lightening_queue;
			ecs::View<ecs::With<block_emmision>, ecs::With<block>> light_view(world);
			for (auto [light, block_component] : light_view) {
				block_component.light_passing_through = stn::max(light.emmision, block_component.light_passing_through);
				lightening_queue.push(block_component);
			}
			for (Chunk::chunk_loaded& load : chunk_loads.read()) {
				for (math::Direction3d dir : math::Directions3d) {
					grid.get_chunk(Chunk::ChunkLocation(load.pos.position + dir.coord()))
						.then([&](Chunk::chunk& cnk) {
						for (Chunk::block_object& object : cnk.on_face(-dir)) {
							lightening_queue.push(object.get<blocks::block>());
						}
						});
				}
			}

			for (lighten_block_command& cmd : world.read_commands<lighten_block_command>()) {
				grid.get_block(cmd.location)
				.then([&](blocks::block& next_block) {lightening_queue.emplace(next_block); });
			}
			while (!lightening_queue.empty()) {
				block& blk = *lightening_queue.pop();
				std::uint16_t light_value = blk.light_passing_through;
				//stop queue at zero 
				if (light_value == 0) {
					continue;
				}
				for (auto dir : math::Directions3d) {

					Coord look_position = blk.pos + dir.coord();
					grid.get_block(look_position).then([&](blocks::block& next_block) {
						if (next_block.mesh.is_transparent()) {
							if (next_block.light_passing_through < light_value - 1) {
								next_block.light_passing_through = light_value - 1;
								lightening_queue.emplace(next_block);
							}
						}
						(next_block)[dir.inverse_index()].set_light(light_value);
					});
				}
			}
		}
	};

	//the advantage of this is that we do not have to modify any visible lights allowing us to not flag for remesh if the change is reverted
	struct FaceLightSetter :ecs::System {
		void run(ecs::Ecs& world) {
			grid::Grid& world_grid = world.get_resource<grid::Grid>();
			for (light_changed& change : world.read_commands< light_changed>()) {
				stn::Option<blocks::block&> mabye_block = world_grid.get_block(change.location);
				if (!mabye_block) {
					continue;
				}
				blocks::block& block_at = mabye_block.unwrap();
				if (block_at.mesh.invisible()) {
					continue;
				}

				if (block_at.mesh.transparent) {
					for (face& fc : block_at.mesh) {
						fc.set_light(block_at.light_passing_through);
					}
				}
				
			}
		}
	};

}
