#pragma once
#include "../game/ecs/component.h"
#include "chunk.h"
#include "chunkload.h"
#include "grid.h"
namespace grid {


	template<grid::WorldAccessor Accessor>
	inline void compute_mesh_cover(block_mesh& mesh, Accessor& grid) {
		
		if (mesh.bounds().scale != blockscale) {
			for (math::Direction3d dir : math::Directions3d) {
				mesh[dir].face().cover = cover_state::Uncovered;
			}
			return;
		}
		for (face& blkface : mesh.faces) {
			if (!blkface.uncomputed()) {
				continue;
			}
			Coord pos = grid.get_voxel(mesh.center()) + blkface.face_direction.coord();
			grid.get_block(pos).then([&](block& adjacent_block) {
				if (mesh.is_transparent()) {
					if (adjacent_block.mesh.is_transparent() && (adjacent_block.mesh[blkface.face_direction.inverse_index()].face().tex == blkface.tex)) {

						blkface.cover = cover_state::Covered;
					}
					else {
						blkface.cover = cover_state::Uncovered;
					}
				}
				else {
					if (adjacent_block.mesh.is_transparent()) {
						blkface.cover = cover_state::Uncovered;
					}
					else {
						blkface.cover = cover_state::Covered;
					}
				}
				});

		}
	}

	struct GridCoverer :ecs::System {
		GridCoverer(ecs::Ecs& world) :event_key(world.make_reader<Chunks::chunk_loaded>()) {

		}

		void run(ecs::Ecs& world) {
			grid::Grid& world_grid = world.get_resource<grid::Grid>();
			for (Chunks::chunk_loaded& cmd : event_key.read()) {
				for (math::Direction3d direction : math::Directions3d) {
					Chunks::ChunkLocation pos = Chunks::ChunkLocation(direction.coord() + cmd.pos.position);
					world_grid.get_chunk_object(pos)
					.then([](grid::ChunkObject& chunk) {
						chunk.get<Chunks::chunkmesh>().recreate_mesh.mark_dirty();
					});
				}
			}
		}
	private:
		ecs::EventReader<Chunks::chunk_loaded> event_key;
	};
}