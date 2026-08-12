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
#include "Lighter.h"
#include "WorldCoverer.h"
#pragma once
namespace grid {
	struct set_block_command {
		Coord pos;
		block_id id;
		math::Direction3d attach_direction;
		set_block_command(Coord position, block_id blk_id, math::Direction3d direction = math::up_3d) :id(blk_id), pos(position),attach_direction(direction){
		}
	};
	inline void dislocate_from_grid(ecs::obj blk, block_id new_id, math::Direction3d attach_direction) {
		grid::Grid& grid = blk.world().get_resource<grid::Grid>();
		auto& blk_comp = blk.get_component<block>();
		auto position = blk_comp.pos;
		ecs::Constrained<block>& to_flip = grid.get_object(position).unwrap();
		to_flip = GenerateBlock{
			.id = new_id,
			.loc = position,
			.block_face = math::up2d,
			.direction = attach_direction,
			.mesh = grid.get_chunk_object(position).unwrap().get<chunks::ChunkMesh>(),
			.registry = blk.world().get_resource<blocks::BlockRegistry>()
		}.spawn(blk.world());

		blk.destroy();
	}
	inline void set_block(ecs::Ecs& world, Coord pos, block_id block_id, math::Direction3d attach_direction= math::up_3d) {
		grid::Grid& grid = world.get_resource<Grid>();
		stn::Option<chunks::block_object&> mabye_location = grid.get_object(pos);
		if (mabye_location) {
			chunks::block_object& location = mabye_location.unwrap();
			grid.get_chunk(pos).unwrap().modified = true;
			size_t old_light = location.get<block>().light_passing_through;
			world.write_command(grid::lighten_block_command(pos));
			world.write_command(partial_darken_command{ .location = pos, .old_light = old_light });
			dislocate_from_grid(location.object(), block_id, attach_direction);
			location = grid.get_object(pos).unwrap();
			blocks::block& blk = location.get<blocks::block>();
			//marks the blocks mesh dirty
			blk.mesh.mark_dirty();
			for (math::Direction3d block_dir : math::Directions3d) {
				grid.get_block(pos + block_dir.coord()).then([&](block& seen_block) {
					seen_block.mesh.uncompute_face_cover(-block_dir);
					world.write_command(grid::lighten_block_command(seen_block.pos));
					});
			}
		}
	}

	struct GridManager :ecs::System {

		GridManager() {
		}
		//removes a block from the grid whilst still keeping it in the work
		




		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
		
			//order of storage for chunks
			//z
			//7,8,9   
			//4,5,6
			//1,2,3-x
			//pattern repeats in the y direction
			grid.grid_pos = grid.get_chunk_pos(grid.center.get_component<core::LocalTransform>().transform.position);
			stn::array<stn::Option<ChunkObject>> newchunklist(grid.totalChunks);
			bool has_unloaded_chunk = false;
			for (size_t ind = 0; ind < grid.totalChunks; ind++) {
				grid.chunklist[ind].then([&](ChunkObject& chnk) {
					chunks::Chunk& Chunk = chnk.get<chunks::Chunk>();
					stn::Option<size_t> new_index = grid.chunk_index(Chunk.location);
					if (new_index) {
						newchunklist[new_index.unwrap_unchecked()] = std::move(chnk);
					}
					else {
						if (!has_unloaded_chunk) {
							grid.need_to_deload.push(std::move(chnk));
						}
					}
					});
			}
			if (grid.need_to_deload.non_empty()) {
				grid.need_to_deload.pop().destroy();
			}
			stn::Option <chunks::ChunkLocation> closest_unloaded_chunk;
			int r = static_cast<int>(grid.rad);
			for (int k = -r; k <= r; k++) {
				for (int j = -r; j <= r; j++) {
					for (int i = -r; i <= r; i++) {
						chunks::ChunkLocation spawn_pos(v3::Coord(i, j, k) + grid.grid_pos.position);
						if (!grid.get_chunk(spawn_pos)) {
							if (!closest_unloaded_chunk || grid.grid_pos.distance_to(spawn_pos) < grid.grid_pos.distance_to(closest_unloaded_chunk.unwrap())) {
								closest_unloaded_chunk = spawn_pos;
							}
						}
					}
				}
			}
			if (closest_unloaded_chunk) {
				chunks::ChunkLocation spawn_location = closest_unloaded_chunk.unwrap();
				ecs::obj chunk_object(world.spawn_empty());
				chunk_object.apply_recipe(world::CreateChunk(world::ChunkLoadInfo{ .location = spawn_location,.generator = *grid.generator }));
				newchunklist[grid.chunk_index(spawn_location).unwrap_unchecked()] = ChunkObject(chunk_object);
			}

			grid.chunklist = std::move(newchunklist);

		}

	};
}