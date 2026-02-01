#pragma once
#include "../game/ecs/multi_query.h"
#include "grid.h"
#include "../block/air.h"
#include "../game/GameContext.h"
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
namespace grid {
	struct light_at {
		light_at(Coord pos, size_t light_value) :position(pos), light(light_value) {
		}
		Coord position;
		size_t light;
	};

	struct partial_reshade_command {
		grid::Grid* grid;
		Coord position;
		partial_reshade_command(grid::Grid& world_grid, Coord pos) :grid(&world_grid), position(pos) {
		}
	};
	struct shade_at {
		shade_at(Coord pos, size_t light_value) :position(pos), shade(light_value) {
		}
		Coord position;
		size_t shade;
	};
	struct GridDarkener :ecs::System {
		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			stn::queue<shade_at> darkening_queue;
			for (partial_reshade_command& cmd : world.read_commands<partial_reshade_command>()) {
				block* blk = grid.getBlock(cmd.position);
				if (blk) {
					darkening_queue.push(shade_at(blk->pos, 15));
				}
			}
			while (!darkening_queue.empty()) {
				shade_at shade = darkening_queue.pop();
				block& blk = *grid.getBlock(shade.position);
				for (auto dir : Dir::Directions3d) {
					Coord look_position = shade.position + dir.to_coord();
					block* blocklight = grid.getBlock(look_position);
					if (blocklight == nullptr) {
						continue;
					}
					if (blocklight->mesh.invisible()) {
						if (blocklight->light_passing_through) {
							blocklight->light_passing_through = stn::None;
							if (shade.shade != 0) {
								darkening_queue.push(shade_at(look_position, shade.shade - 1));
							}
						}
					}
					else {
						(*blocklight)[dir.inv_ind()].set_light(0);
					}

				}
			}
		}
	};

	struct SpreadLightCommand {


	};
	struct GridLighter :ecs::System {
		ecs::EventReader<Chunk::chunk_loaded> chunk_loads;
		GridLighter(ecs::Ecs& world) :chunk_loads(world.make_reader<Chunk::chunk_loaded>()) {
		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			stn::queue<light_at> lightening_queue;
			ecs::View<block_emmision, block> light_view(world);
			for (const auto& [light, block_component] : light_view) {
				lightening_queue.emplace(block_component.pos, light.emmision);
			}
			while (!lightening_queue.empty()) {
				light_at light = lightening_queue.pop();
				block* blk_m = grid.getBlock(light.position);
				if (!blk_m) {
					//because blocks that are currenntlly being deleted 
					continue;
				}
				block& blk = *blk_m;
				size_t light_value = blk.light_passing_through.unwrap();
				for (auto dir : Dir::Directions3d) {

					Coord look_position = light.position + dir.to_coord();
					block* blocklight = grid.getBlock(look_position);

					if (blocklight == nullptr) {
						continue;
					}
					if (blocklight->mesh.invisible()) {
						if (blocklight->light_passing_through.is_none_or([light_value](std::uint8_t light) {return light < light_value - 1; })) {
							blocklight->light_passing_through = light_value - 1;
							if (blocklight->light_passing_through != 0) {
								lightening_queue.emplace(look_position, blocklight->light_passing_through.unwrap());
							}
						}
					}
					(*blocklight)[dir.inv_ind()].set_light(light_value);
				}
			}
		}
	};

}