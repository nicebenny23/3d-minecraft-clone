#pragma once
#include "../game/ecs/component.h"
#include "chunk.h"
#include "chunkload.h"
#include "grid.h"
namespace grid {


	struct cover_block_command {

		Coord block_location;
	};


	struct recompute_chunk_cover {
		recompute_chunk_cover(ecs::obj cnk) :chunk(cnk) {

		};
		ecs::obj chunk;
	};
	struct GridCoverer :ecs::System {
		GridCoverer(ecs::Ecs& world) :event_key(world.make_reader<Chunk::chunk_loaded>()) {

		}
		void compute_mesh_cover(blockmesh& mesh, Grid& grid) {
			if (mesh.invisible()) {
				for (size_t i = 0; i < 6; i++) {
					mesh[i].set_cover(cover_state::Covered);
				}
				return;
			}
			if (mesh.scale()!=v3::Scale3(blockscale)) {
				for (size_t i = 0; i < 6; i++) {
					mesh[i].set_cover(cover_state::Uncovered);
				}
				return;
			}
			for (face& blkface : mesh.faces) {

				if (!blkface.uncomputed()) {
					continue;
				}

				Coord pos = mesh.mesh_location + blkface.face_direction.coord();
				block* adjacent_block = grid.getBlock(pos);
				if (adjacent_block == nullptr) {
					continue;
				}
				if (mesh.is_transparent()) {
					if (adjacent_block->mesh.is_transparent() && (adjacent_block->mesh[blkface.face_direction.inverse_index()].tex == blkface.tex)) {
						blkface.set_cover(cover_state::Covered);
					}
					else {
						blkface.set_cover(cover_state::Uncovered);
					}
				}
				else {
					if (adjacent_block->mesh.is_transparent()) {
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
			for (ecs::obj blk : cnk.block_list) {
				compute_mesh_cover(blk.get_component<block>().mesh, chunk.world().get_resource<Grid>().expect("grid should exist"));
			}
		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>().unwrap();
			for (cover_block_command& cmd : world.read_commands<cover_block_command>()) {
				block* blk = grid.getBlock(cmd.block_location);
				if (!blk) {
					continue;
				}
				for (face& fc : blk->mesh) {
					fc.set_cover(cover_state::Uncomputed);
				}
				compute_mesh_cover(blk->mesh, world.get_resource<Grid>().expect("grid should exist"));
			}

			for (Chunk::chunk_loaded& cmd : event_key.read()) {
				if (grid.get_chunk(cmd.pos)) {
					world.emplace_command<recompute_chunk_cover>(grid.get_chunk(cmd.pos).unwrap().owner());
				}
				for (math::Direction3d direction : math::Directions3d) {
					Chunk::ChunkLocation pos = Chunk::ChunkLocation(direction.coord() + cmd.pos.position);
					if (grid.get_chunk(pos)) {
						world.emplace_command<recompute_chunk_cover>(grid.get_chunk(pos).unwrap().owner());
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
}