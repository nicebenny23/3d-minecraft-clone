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
namespace gridutil {
	struct set_block_command {
		Coord pos;
		char block_id;
		set_block_command(Coord position, char blk_id) :block_id(blk_id), pos(position) {
		}
	};
	inline void set_block(Coord pos, char block_id) {
		CtxName::ctx.Ecs->write_command<set_block_command>(set_block_command(pos, block_id));
	}

	struct complete_relight_command {
		grid::Grid* grid;
		complete_relight_command(grid::Grid* world_grid) :grid(world_grid) {


		}

	};
	struct partial_relight_command {
		grid::Grid* grid;
		Coord position;
		partial_relight_command(grid::Grid& world_grid, Coord pos) :grid(&world_grid), position(pos) {


		}
	};
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
					if (blocklight->attributes.transparent) {

						if (blocklight->lightval) {
							if (blocklight->emitedlight != 0) {
								world.write_command(partial_relight_command(grid, blocklight->pos));
							}
							blocklight->lightval = stn::None;
						
							if (shade.shade != 0) {
								darkening_queue.push(shade_at(look_position, shade.shade - 1));
							}

						}

					}
					else {
						int blockface = dir.Inv().ind();
						(*blocklight)[blockface].light = 0;

					}

				}
			}
		}
	};




	struct GridLighter :ecs::System {

		GridLighter() {
		}

		void collect_if_light(std::tuple<blockname::block&> block, stn::mutex<stn::queue<light_at>>& push_mutex) {
			auto& [blk] = block;
			for (auto dir : Dir::Directions3d) {
				(blk.mesh.faces)[dir.ind()].light = 0;
			}
			blk.lightval = blk.emitedlight;
			if (0 < blk.emitedlight) {
				
				push_mutex.modify([&blk](stn::queue<light_at>& lightingq) {
					lightingq.emplace(blk.pos, blk.emitedlight);

					});
			}

		}
		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			stn::queue<light_at> lightening_queue;
			stn::dirty_flag redo_lighting(false);
			for (complete_relight_command& cmd : world.read_commands<complete_relight_command>()) {
				redo_lighting.mark_dirty();
			}
			bool has_cleaned = redo_lighting.clean([&]() {
				stn::mutex<stn::queue<light_at>> fill_mutex{ stn::queue<light_at>() };
				ecs::View<block> blk(world);

				ecs::multi_query(blk, 4, [&fill_mutex, this](std::tuple<blockname::block&> block) {
					collect_if_light(block, fill_mutex);
					});
				std::move(fill_mutex).take([&](auto&& value) {lightening_queue = (std::move(value)); });
				});


			for (partial_relight_command& cmd : world.read_commands<partial_relight_command>()) {
				block* blk = grid.getBlock(cmd.position);
				if (blk) {
					blk->lightval = blk->emitedlight;
					lightening_queue.emplace(cmd.position, blk->emitedlight);
				}
			}



			while (!lightening_queue.empty()) {
				light_at light = lightening_queue.pop();
				block* blk_m = grid.getBlock(light.position);
				if (!blk_m) {
				//because blocks that are currenntlly being deleted 
					continue;
				}
				block& blk = *blk_m;
				size_t light_value = blk.lightval.unwrap();
				for (auto dir : Dir::Directions3d) {

					Coord look_position = light.position + dir.to_coord();
					block* blocklight = grid.getBlock(look_position);
					
					if (blocklight == nullptr) {
						continue;
					}
					if (blocklight->attributes.transparent) {

						if (blocklight->lightval.is_none_or([light_value](std::uint8_t light) {return light < light_value - 1;})) {
							if (light_value!=0) {
								blocklight->lightval = light_value - 1;
								lightening_queue.emplace(look_position, blocklight->lightval.unwrap());
							}
						}
					}
					else {
						int blockface = dir.Inv().ind();
						(*blocklight)[blockface].light = light_value;
					}

				}

			}

		}

	};
	struct cover_block_command {

		Coord block_location;
	};
	struct recompute_cover_event {
		recompute_cover_event(grid::Grid& compute_for) :grid(compute_for) {

		}
		grid::Grid& grid;
	};

	struct GridCoverer :ecs::System {
		GridCoverer() {
		}

		void computecover(face& blkface) {

			if (blkface.cover != cover_state::Uncomputed) {
				return;
			}
			if (blkface.mesh->invisible()) {
				blkface.cover = cover_state::Covered;
				return;
			}
			if (blkface.mesh->scale()[blkface.facenum.axis_index()] != block_axis_scale) {
				blkface.cover = cover_state::Uncovered;
				return;
			}
			block& current_blk= *blkface.mesh->blk;

			Coord pos = current_blk.pos + blkface.facenum.to_coord();
			block* adjacent_block = CtxName::ctx.Grid->getBlock(pos);
			if (adjacent_block == nullptr) {
				blkface.cover = cover_state::Uncomputed;
				return;
			}
			if (current_blk.attributes.transparent) {
					if (adjacent_block->attributes.transparent && (adjacent_block->id == current_blk.id)) {
						blkface.cover = cover_state::Covered;
					}
					else {
						blkface.cover = cover_state::Uncovered;
					}
			}
			else {
				if (adjacent_block->attributes.transparent) {

					blkface.cover = cover_state::Uncovered;
				}
				else {
					blkface.cover = cover_state::Covered;
				}
			}
		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			for (cover_block_command& cmd : world.read_commands<cover_block_command>()) {
				block* blk = grid.getBlock(cmd.block_location);
				if (!blk) {
					continue;
				}
				for (int faceind = 0; faceind < 6; faceind++) {
					(blk->mesh)[faceind].cover = cover_state::Uncomputed;
					computecover((blk->mesh)[faceind]);
				}
			}
			bool should_recompute_entire = false;
			for (recompute_cover_event& cmd : world.read_commands<recompute_cover_event>()) {
				should_recompute_entire = true;
			}
			if (should_recompute_entire) {
				ecs::View<blockname::block> blk(world);
				ecs::multi_query(blk, 10, [this](std::tuple<blockname::block&> Block) {

					blockname::block& blk = std::get<0>(Block);

					for (size_t faceind = 0; faceind < 6; ++faceind) {
						face& tocover = blk[faceind];
						computecover(tocover);

					}

					});
			}
		}
	};

	struct GridManager :ecs::System {

		GridManager() {
		}
		//removes a block from the grid whilst still keeping it in the work
		ecs::obj dislocate(ecs::obj blk) {
			auto& blk_comp = blk.get_component<block>();
			auto position = blk_comp.pos;
			ecs::obj& to_flip = *CtxName::ctx.Grid->getObject(position);
			to_flip = CtxName::ctx.Ecs->spawn_empty();
			to_flip.add_component<block>().create(position, minecraftair, blk_comp.mesh.attachdir, blk_comp.mesh.direction);
			setdefault(to_flip.get_component<block>());
			blk.remove_component<block>();
			return blk;
		}

		void set_air(ecs::obj blk) {
			dislocate(blk).destroy();
		}


		void blockchangecoverupdate(ecs::Ecs& world, blockname::block* location) {
			world.write_command<cover_block_command>(cover_block_command(location->pos));
			for (Dir::Dir3d block_dir : Dir::Directions3d) {
				world.write_command<cover_block_command>(cover_block_command(location->pos + block_dir.to_coord()));
			}
		}

		
		//the set_block function is what should be used
		void set_block(Coord loc, int blockid, grid::Grid& grid);


		void sendrecreatemsg(grid::Grid& grid) {
			for (int i = 0; i < grid.totalChunks; i++) {
				if (grid[i] != nullptr) {
					grid[i]->mesh->recreate_mesh.mark_dirty();
				}
			}

		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			for (set_block_command& cmd : world.read_commands<set_block_command>()) {
				set_block(cmd.pos, cmd.block_id, grid);
			}
			grid.updateborders();
			grid.load();
			if (grid.haschanged()) {
				world.write_command(recompute_cover_event(grid));
				world.write_command(complete_relight_command(CtxName::ctx.Grid));
				sendrecreatemsg(grid);
			}
		}
	};
}