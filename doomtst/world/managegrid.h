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
#include "Lighter.h"
#pragma once
namespace gridutil {
	struct set_block_command {
		Coord pos;
		char block_id;
		set_block_command(Coord position, char blk_id) :block_id(blk_id), pos(position) {
		}
	};
	inline void set_block(ecs::Ecs& world, Coord pos, char block_id) {
		world.write_command<set_block_command>(set_block_command(pos, block_id));
	}

	


	struct cover_block_command {

		Coord block_location;
	};


	struct recompute_chunk_cover {
		recompute_chunk_cover(ecs::obj cnk) :chunk(cnk) {

		};
		ecs::obj chunk;
	};
	struct GridCoverer :ecs::System {
		GridCoverer(ecs::Ecs& world):event_key(world.make_reader<Chunk::chunk_loaded>()) {
		
		}
		void compute_mesh_cover(blockmesh& mesh) {
			if (mesh.invisible()|| mesh.scale() != v3::Scale3(block_axis_scale)) {
				for (size_t i = 0; i < 6; i++) {
					mesh[i].set_cover(cover_state::Covered);
				}
				return;
			}

			for (face& blkface : mesh.faces) {

				if (blkface.uncomputed()) {
					continue;
				}

				Coord pos = mesh.mesh_location + blkface.facenum.to_coord();
				block* adjacent_block = CtxName::ctx.Grid->getBlock(pos);
				if (adjacent_block == nullptr) {
					blkface.uncomputed();
					continue;
				}
				if (mesh.invisible()) {
					if (adjacent_block->mesh.invisible() && (adjacent_block->mesh.render_id == mesh.render_id)) {
						blkface.set_cover(cover_state::Covered);
					}
					else {
						blkface.set_cover(cover_state::Uncovered);
					}
				}
				else {
					if (adjacent_block->mesh.invisible()) {
						blkface.set_cover(cover_state::Uncovered);
					}
					else {
						blkface.set_cover(cover_state::Covered);
					}
				}
				
			}
		}

		void compute_chunk_cover(ecs::obj chunk) {
			Chunk::chunk& cnk = chunk.get_component<Chunk::chunk >();
			for (ecs::obj blk:cnk.block_list) {
				compute_mesh_cover(blk.get_component<block>().mesh);
			}
		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			for (cover_block_command& cmd : world.read_commands<cover_block_command>()) {
				block* blk = grid.getBlock(cmd.block_location);
				if (!blk) {
					continue;
				}
				compute_mesh_cover(blk->mesh);
			}
			
			for (Chunk::chunk_loaded& cmd : event_key.read()) {
				if (grid.GetChunk(cmd.pos)) {
					world.emplace_command<recompute_chunk_cover>(grid.GetChunk(cmd.pos)->owner());
				}
				for (Dir::Dir3d direction : Dir::Directions3d) {
					v3::Coord pos = direction.to_coord() + cmd.pos;
					if (grid.GetChunk(pos)) {
						world.emplace_command<recompute_chunk_cover>(grid.GetChunk(pos)->owner());
					}
				}
			}
			stn::array<ecs::obj> chunks_to_recompute;

			for (recompute_chunk_cover cmd : world.read_commands<recompute_chunk_cover>()) {
				if (!chunks_to_recompute.contains(cmd.chunk)) {
					chunks_to_recompute.push(cmd.chunk);
				}
			}
			if (chunks_to_recompute.nonempty()) {

				thread_util::par_iter_rng(chunks_to_recompute, [&](ecs::obj elem) {compute_chunk_cover(elem); }, 4);
			}

		}
	private:
		ecs::EventReader<Chunk::chunk_loaded> event_key;
	};
	struct chunk_mesh_dirty_event {
		ecs::obj chunk;
	};

	struct GridManager :ecs::System {

		GridManager() {
		}
		//removes a block from the grid whilst still keeping it in the work
		ecs::obj dislocate_from_grid(ecs::obj blk,block_id new_id) {
			auto& blk_comp = blk.get_component<block>();
			auto position = blk_comp.pos;
			ecs::obj& to_flip = *CtxName::ctx.Grid->getObject(position);
			to_flip = ecs::spawn_emplaced<GenerateBlock>( blk.world(), CtxName::ctx.Grid->GetChunk(position)->owner().get_component<Chunk::chunkmesh>(), new_id, position, blk_comp.mesh.direction, blk_comp.mesh.attachdir);
			setdefault(to_flip.get_component<block>());
			blk.remove_component<block>();
			return blk;
		}



		void blockchangecoverupdate(ecs::Ecs& world, blocks::block* location) {
			world.write_command<cover_block_command>(cover_block_command(location->pos));
			for (Dir::Dir3d block_dir : Dir::Directions3d) {
				world.write_command<cover_block_command>(cover_block_command(location->pos + block_dir.to_coord()));
			}
		}


		//the set_block function is what should be used
		void set_block(Coord loc, block_id blockid, grid::Grid& grid, ecs::Ecs& world);


		

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			for (set_block_command& cmd : world.read_commands<set_block_command>()) {
				set_block(cmd.pos, cmd.block_id, grid, world);
			}
			grid.updateborders();
			grid.load(world);
			}
	
	};
}