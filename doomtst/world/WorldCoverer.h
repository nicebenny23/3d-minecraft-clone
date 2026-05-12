#pragma once
#include "../game/ecs/component.h"
#include "chunk.h"
#include "chunkload.h"
#include "grid.h"
namespace grid {


	template<grid::WorldAccessor Accessor>
	inline void compute_mesh_cover(BlockMesh& mesh, Accessor& grid) {
		
		if (mesh.bounds().scale != blockscale) {
			for (math::Direction3d dir : math::Directions3d) {
				mesh[dir].face().cover = cover_state::Uncovered;
			}
			return;
		}
		Coord center = grid.get_voxel(mesh.center());
		for (math::Direction3d& direction:math::Directions3d) {
			blocks::BlockFace& blkface = mesh.faces[direction.index()];
			if (!blkface.uncomputed()) {
				continue;
			}
			Coord pos = center+direction.coord();
			grid.get_block(pos).then([&](block& adjacent_block) {
				if (mesh.is_transparent()) {
					if (adjacent_block.mesh.is_transparent() && (adjacent_block.mesh[direction.inverse_index()].face().tex == blkface.tex)) {

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
		GridCoverer(ecs::Ecs& world) :event_key(world.make_reader<world::ChunkLoaded>()) {

		}

		void run(ecs::Ecs& world) {
			grid::Grid& world_grid = world.get_resource<grid::Grid>();
			for (world::ChunkLoaded& cmd : event_key.read()) {
				for (math::Direction3d direction : math::Directions3d) {
					chunks::ChunkLocation pos = chunks::ChunkLocation(direction.coord() + cmd.pos.position);
					world_grid.get_chunk_object(pos)
					.then([](grid::ChunkObject& Chunk) {
						Chunk.get<chunks::ChunkMesh>().recreate_mesh.mark_dirty();
					});
				}
			}
		}
	private:
		ecs::EventReader<world::ChunkLoaded> event_key;
	};
}