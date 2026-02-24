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
		void compute_mesh_cover(block_mesh& mesh, Grid& grid) {
			if (mesh.invisible()) {
				return;
				//we can skip all covering in this case
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

				Coord pos = grid.get_voxel(mesh.center()) + blkface.face_direction.coord();
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

		void compute_chunk_cover(ecs::Constrained<Chunk::chunk> chunk) {
			grid::Grid& grid = chunk.world().get_resource<Grid>();
			Chunk::chunk& cnk = chunk.get<Chunk::chunk >();
			for (auto& blk : cnk.block_list) {
				compute_mesh_cover(blk.get<block>().mesh, grid);
			}
		}

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			for (cover_block_command& cmd : world.read_commands<cover_block_command>()) {
				block* blk = grid.getBlock(cmd.block_location);
				if (!blk) {
					continue;
				}
				for (face& fc : blk->mesh) {
					fc.set_cover(cover_state::Uncomputed);
				}
				compute_mesh_cover(blk->mesh, world.get_resource<Grid>());
			}

			for (Chunk::chunk_loaded& cmd : event_key.read()) {
				if (grid.get_chunk(cmd.pos)) {
					compute_chunk_cover(grid.get_chunk(cmd.pos).unwrap().owner());
				}
				for (math::Direction3d direction : math::Directions3d) {
					Chunk::ChunkLocation pos = Chunk::ChunkLocation(direction.coord() + cmd.pos.position);
					grid.get_chunk(pos).then([&](Chunk::chunk& chunk) {
						//for eac
						for (Chunk::block_object& object : chunk.on_face(-direction)) {
							compute_mesh_cover(object.get<block>().mesh, grid);
						}
					});
				}
			}
		}
	private:
		ecs::EventReader<Chunk::chunk_loaded> event_key;
	};
	struct chunk_mesh_dirty_event {
		ecs::obj chunk;
	};
}