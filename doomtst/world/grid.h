#include "../game/camera.h"
#include "../game/ecs/filtered_object.h"
#include "../math/geometry.h"
#include "../math/vector3.h"
#include "chunkload.h"
#pragma once
namespace grid {


	using ChunkObject = ecs::ConstrainedHandle<Chunk::chunk, Chunk::chunkmesh>;
	struct Grid :ecs::resource {
		//total length in one dimention
		const size_t rad;
		//how many chunks it spans from the center chunk 
		int dim_axis;
		size_t totalChunks;
		Chunk::ChunkLocation grid_pos;
		stn::array<stn::Option<ChunkObject>> chunklist;

		Grid(size_t axis) :rad(axis), dim_axis(2 * axis + 1), grid_pos(v3::ZeroCoord) {
			totalChunks = dim_axis * dim_axis * dim_axis;
			chunklist = stn::array<stn::Option<ChunkObject>>(totalChunks);
		}
		//order of storage for chunks
		//z
		//7,8,9   
		//4,5,6
		//1,2,3-x
		//pattern repeats in the y direction
		void load(ecs::Ecs& world) {
			stn::array<stn::Option<ChunkObject>> newchunklist(totalChunks);
			bool has_unloaded_chunk = false;
			for (size_t ind = 0; ind < totalChunks; ind++) {
				chunklist[ind].then([&](ChunkObject& chnk) {
					Chunk::chunk& chunk = chnk.get<Chunk::chunk>();
					stn::Option<size_t> new_index = chunk_index(chunk.location);
					if (new_index) {
						newchunklist[new_index.unwrap_unchecked()] = std::move(chnk);
					}
					else {
						if (!has_unloaded_chunk) {
							need_to_deload.push(std::move(chnk));
						}
					}
					});
			}
			if (need_to_deload.non_empty()) {
				need_to_deload.pop().destroy();
			}
			stn::Option <Chunk::ChunkLocation> closest_unloaded_chunk;
			int r = static_cast<int>(rad);
			for (int k = -r; k <= r; k++) {
				for (int j = -r; j <= r; j++) {
					for (int i = -r; i <= r; i++) {
						Chunk::ChunkLocation spawn_pos(v3::Coord(i, j, k) + grid_pos.position);
						if (!get_chunk(spawn_pos)) {

							if (!closest_unloaded_chunk || grid_pos.distance_to(spawn_pos) < grid_pos.distance_to(closest_unloaded_chunk.unwrap())) {
								closest_unloaded_chunk = spawn_pos;
							}
						}
					}
				}
			}
			if (closest_unloaded_chunk) {
				Chunk::ChunkLocation spawn_location = closest_unloaded_chunk.unwrap();
				struct chunk_tag {

				};
				ecs::obj chunk_object(world.spawn_tagged<chunk_tag>(), world);
				chunk_object.apply_recipe(Chunk::CreateChunk(spawn_location));
				newchunklist[chunk_index(spawn_location).unwrap_unchecked()] = ChunkObject(chunk_object);
			}

			chunklist = std::move(newchunklist);
		}

		void updateborders() {
			Point3 pos = camera::campos();
			grid_pos = get_chunk_pos(pos);
		}


		Point3 to_block_pos(Point3 point) {
			return Point3(point.x / blocksize, point.y / blocksize, point.z / blocksize);
		}
		Chunk::ChunkLocation chunk_from_block_pos(Coord pos) {
			return  Chunk::ChunkLocation::from_block_pos(pos);
		}
		Coord get_voxel(Point3 pos) const {
			return  Coord(std::floor(pos.x / blocksize), std::floor(pos.y / blocksize), std::floor(pos.z / blocksize));
		}
		Chunk::ChunkLocation get_chunk_pos(Point3 pos) {
			return chunk_from_block_pos(get_voxel(pos));
		}
		bool contains_chunk_location(Chunk::ChunkLocation loc) const {
			loc.position -= grid_pos.position;
			return (abs(loc.position.x) <= rad && abs(loc.position.y) <= rad && abs(loc.position.z) <= rad);
		}
		stn::Option<size_t> chunk_index(Chunk::ChunkLocation pos) const {
			pos.position -= grid_pos.position;
			if (abs(pos.position.x) <= rad && abs(pos.position.y) <= rad && abs(pos.position.z) <= rad) {
				//now spans
				pos.position += Coord(rad, rad, rad);
				return pos.position.x + pos.position.y * dim_axis + pos.position.z * dim_axis * dim_axis;
			}
			return stn::None;
		}

		stn::Option<ChunkObject&> get_chunk_object(Chunk::ChunkLocation pos) {
			stn::Option < size_t> index = chunk_index(pos);
			if (index) {

				auto& idk = chunklist.unchecked_at(index.unwrap_unchecked());
				if (idk) {
					return idk.unwrap_unchecked();
				}
			}
			return stn::None;
		}
		stn::Option<Chunk::chunk&> get_chunk(Chunk::ChunkLocation pos) {
			return get_chunk_object(pos)
				.map([&](ChunkObject& object)->Chunk::chunk& {return object.get_unchecked<Chunk::chunk>(); });
		}

		stn::Option<Chunk::chunk&> get_chunk(v3::Coord pos) {
			return get_chunk(chunk_from_block_pos(pos));
		}
		stn::Option<ChunkObject&> get_chunk_object(v3::Coord pos) {
			return get_chunk_object(chunk_from_block_pos(pos));
		}
		Option<ecs::Constrained<block>&> get_object(v3::Coord pos) {
			return get_chunk(pos).map([&](Chunk::chunk& chnk)->ecs::Constrained<block>& {return chnk.unchecked_at_pos(pos); });
		}
		Option<block&> get_block(v3::Coord pos) {
			return get_object(pos)
				.map([](Chunk::block_object& block_object)->block& {return block_object.get_unchecked<block>(); });
		}
		array<ecs::obj > voxel_in_range(geo::Box span) {
			array<ecs::obj> blocks;
			v3::Coord lowest = get_voxel(span.min());
			v3::Coord highest = get_voxel(span.max());
			for (int x = lowest.x; x <= highest.x; x++) {
				for (int y = lowest.y; y <= highest.y; y++) {
					for (int z = lowest.z; z <= highest.z; z++) {
						stn::Option<ecs::obj> blk = get_object(Coord(x, y, z)).map_member(&ecs::Constrained<block>::object);
						if (blk) {
							blocks.push(blk.unwrap());
						}
					}
				}
			}
			return blocks;
		}

		size_t chunks_loaded() {
			return chunklist.pipe().count([](const stn::Option<ChunkObject>& object) {return object.is_some(); });
		}
		stn::array<ChunkObject> need_to_deload;


	};

	//supports really fast grid acesses in a chunk by catching the chunk
	struct FocusedGridAcessor {
		stn::Option<block&> get_block(Coord pos) {
			if (chunk.location.contains_block(pos)) {
				return chunk.unchecked_at_pos(pos).get_unchecked<block>();
			}
			return world.get_block(pos);
		}
		FocusedGridAcessor(ChunkObject::ObjectType chunk_object, grid::Grid& grid) :world(grid), chunk(chunk_object.get<Chunk::chunk>()) {

		}
		FocusedGridAcessor(ChunkObject::ObjectType chunk_object) :world(chunk.world().get_resource<grid::Grid>()), chunk(chunk_object.get<Chunk::chunk>()) {

		}
		Coord get_voxel(v3::Point3 point) {
			return world.get_voxel(point);
		}
	private:
		Chunk::chunk& chunk;
		grid::Grid& world;

	};
	template<typename T>
	concept WorldAccessor = requires(T & accessor, v3::Coord pos) {
		{
			accessor.get_block(pos)
		}->std::same_as<stn::Option<block&>>;
	};
};

