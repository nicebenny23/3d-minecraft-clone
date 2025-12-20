#pragma once
#include "../game/ecs/multi_query.h"
#include "grid.h"
#include "../block/air.h"
#include "../game/GameContext.h"
#include "../util/mutex.h"
#include "../util/cached.h"
namespace gridutil {
	struct set_block_command {
		Coord pos;
		char block_id;
		set_block_command(Coord position, char blk_id) :block_id(blk_id), pos(position) {
		}
	};
	struct complete_relight_command {
		grid::Grid* grid;
		complete_relight_command(grid::Grid* world_grid) :grid(world_grid) {


		}

	};
	struct partial_relight_command {

		grid::Grid* grid;
		Coord position;
		partial_relight_command(grid::Grid* world_grid, Coord pos) :grid(world_grid), position(pos) {


		}
	};
	inline void set_block(Coord pos, char block_id) {
		CtxName::ctx.Ecs->write_command<set_block_command>(set_block_command(pos, block_id));
	}
	struct GridCoverer;
	struct GridLighter :ecs::System {

		GridLighter(){
		}
	
		void collect_if_light(std::tuple<blockname::block&> block, stn::mutex<stn::queue<blockname::block*>>& push_mutex) {
			auto& [blk] = block;
			for (auto dir : Dir::Directions3d) {
				(blk.mesh.faces)[dir.ind()].light = 0;

			}
			blk.lightval = blk.emitedlight;
			if (0 < blk.emitedlight) {
				push_mutex.modify([&blk](stn::queue<blockname::block*>& lightingq) {
					lightingq.push(&blk);
				
					});
			}

		}
		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			stn::queue<blockname::block*> lighting_queue;
			stn::dirty_flag redo_lighting(false);
			for (complete_relight_command& cmd : world.read_commands<complete_relight_command>()) {
				redo_lighting.mark_dirty();
			}
			for (partial_relight_command& cmd : world.read_commands<partial_relight_command>()) {
				block* blk = grid.getBlock(cmd.position);

				blk->lightval = blk->emitedlight;
				lighting_queue.push(blk);
			}
			redo_lighting.clean([&]() {
				stn::mutex<stn::queue<blockname::block*>> fill_mutex{ stn::queue<blockname::block*>() };
				ecs::View<block> blk(world);

				ecs::multi_query(blk, 4, [&fill_mutex, this](std::tuple<blockname::block&> block) {
					collect_if_light(block, fill_mutex);
					});
				std::move(fill_mutex).take([&](auto&& value) {lighting_queue = (std::move(value)); });
				});

			while (!lighting_queue.empty()) {
				block* blk = lighting_queue.pop();
				if (!blk->attributes.transparent) {
					continue;
				}
				for (auto dir : Dir::Directions3d) {
					block* blocklight = grid.getBlock(blk->pos + dir.to_coord());
					if (blocklight == nullptr) {
						continue;
					}
					if (blocklight->attributes.transparent) {

						if (blocklight->lightval < blk->lightval - 1) {
							blocklight->lightval = blk->lightval - 1;
							lighting_queue.push(blocklight);
						}
					}
					else {
						int blockface = dir.Inv().ind();
						(*blocklight)[blockface].light = blk->lightval;
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
			if (blkface.cover!=cover_state::Uncomputed) {
				return;
			}
			if (blkface.mesh->invisible()) {
				blkface.cover = cover_state::Covered;
				return;
			}
			if (blkface.mesh->scale()[blkface.facenum.axis_index()]!= block_axis_scale) {
				blkface.cover = cover_state::Uncovered;
				return;
			}
			Coord pos = blkface.mesh->blk->pos + blkface.facenum.to_coord();
			block* blk = CtxName::ctx.Grid->getBlock(pos);
			if (blk == nullptr) {
				blkface.cover = cover_state::Uncomputed;
				return;
			}
			if (blkface.mesh->blk->attributes.transparent) {
				if (blkface.mesh->blk->id != minecraftair) {
					if (blk->attributes.transparent && (blk->id == blkface.mesh->blk->id)) {
						blkface.cover = cover_state::Covered;
					}
					else {
						blkface.cover = cover_state::Uncovered;
					}
				}
			}
			else {
				if (blk->attributes.transparent) {
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
					
						blockname::block& blk=std::get<0>(Block);
						
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
			setdefault(&to_flip.get_component<block>());
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

		void blocklightingupdateevent(int prevlight, int newlight, Coord loc) {
			if (newlight >= prevlight) {
				
				CtxName::ctx.Ecs->write_command(partial_relight_command(CtxName::ctx.Grid, loc));
				for (Dir::Dir3d direction : Dir::Directions3d) {
					CtxName::ctx.Ecs->write_command(partial_relight_command(CtxName::ctx.Grid, loc + direction.to_coord()));
				}
			}
			else {
				CtxName::ctx.Ecs->write_command(complete_relight_command(CtxName::ctx.Grid));
			}

		}
		//the set_block function is what should be used
		void set_block(Coord loc, int blockid,grid::Grid& grid);


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
				set_block(cmd.pos, cmd.block_id,grid);
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