
#include "../game/ecs/filtered_object.h"
#include "../math/geometry.h"
#include "../math/vector3.h"
#include "chunkload.h"
#pragma once
namespace grid {


	using ChunkObject = ecs::ConstrainedHandle<chunks::Chunk, chunks::ChunkMesh>;
	struct Grid :ecs::resource {
		//total length in one dimention
		const size_t rad;
		//how many chunks it spans from the center chunk 
		size_t dim_axis;
		size_t totalChunks;

		geo::Box bounds() const {
			return geo::Box(grid_pos.center(), v3::Scale3::from_scale(dim_axis* chunks::chunk_length));
		}
		chunks::ChunkLocation grid_pos;
		stn::array<stn::Option<ChunkObject>> chunklist;
		ecs::Constrained<core::LocalTransform> center;
		stn::box<world::TerrainGenerator> generator;
		Grid(size_t axis, ecs::Constrained<core::LocalTransform> follow, stn::box<world::TerrainGenerator>&& terrain_generator) :rad(axis),generator(std::move(terrain_generator)), dim_axis(2 * axis + 1), grid_pos(v3::ZeroCoord), center(follow) {
			totalChunks = dim_axis * dim_axis * dim_axis;
			chunklist = stn::array<stn::Option<ChunkObject>>(totalChunks);
			//hack
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
					chunks::Chunk& Chunk = chnk.get<chunks::Chunk>();
					stn::Option<size_t> new_index = chunk_index(Chunk.location);
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
			stn::Option <chunks::ChunkLocation> closest_unloaded_chunk;
			int r = static_cast<int>(rad);
			for (int k = -r; k <= r; k++) {
				for (int j = -r; j <= r; j++) {
					for (int i = -r; i <= r; i++) {
						chunks::ChunkLocation spawn_pos(v3::Coord(i, j, k) + grid_pos.position);
						if (!get_chunk(spawn_pos)) {

							if (!closest_unloaded_chunk || grid_pos.distance_to(spawn_pos) < grid_pos.distance_to(closest_unloaded_chunk.unwrap())) {
								closest_unloaded_chunk = spawn_pos;
							}
						}
					}
				}
			}
			if (closest_unloaded_chunk) {
				chunks::ChunkLocation spawn_location = closest_unloaded_chunk.unwrap();
				struct chunk_tag {

				};
				ecs::obj chunk_object(world.spawn_empty());
				chunk_object.apply_recipe(world::CreateChunk(world::ChunkLoadInfo{ .location = spawn_location,.generator = *generator }));
				newchunklist[chunk_index(spawn_location).unwrap_unchecked()] = ChunkObject(chunk_object);
			}

			chunklist = std::move(newchunklist);
		}

		void updateborders() {
			Point3 pos = center.get_component<core::LocalTransform>().transform.position;
			grid_pos = get_chunk_pos(pos);
		}


		Point3 to_block_pos(Point3 point) {
			return Point3(point.x / blocksize, point.y / blocksize, point.z / blocksize);
		}
		//keeping here because I might make chunk locations assigned t oa unique world in the futurw
		chunks::ChunkLocation chunk_from_block_pos(Coord pos) const {
			return  chunks::ChunkLocation::from_block_pos(pos);
		}
		Coord get_voxel(Point3 pos) const {
			return  Coord(std::floor(pos.x / blocksize), std::floor(pos.y / blocksize), std::floor(pos.z / blocksize));
		}
		chunks::ChunkLocation get_chunk_pos(Point3 pos) {
			return chunk_from_block_pos(get_voxel(pos));
		}
		bool contains_chunk_location(chunks::ChunkLocation loc) const {
			loc.position -= grid_pos.position;
			return (abs(loc.position.x) <= rad && abs(loc.position.y) <= rad && abs(loc.position.z) <= rad);
		}
		stn::Option<size_t> chunk_index(chunks::ChunkLocation pos) const {
			pos.position -= grid_pos.position;
			if (abs(pos.position.x) <= rad && abs(pos.position.y) <= rad && abs(pos.position.z) <= rad) {
				//now spans
				pos.position += Coord(rad, rad, rad);
				return pos.position.x + pos.position.y * dim_axis + pos.position.z * dim_axis * dim_axis;
			}
			return stn::None;
		}

		stn::Option<ChunkObject&> get_chunk_object(chunks::ChunkLocation pos) {
			stn::Option < size_t> index = chunk_index(pos);
			if (index) {

				auto& idk = chunklist.unchecked_at(index.unwrap_unchecked());
				if (idk) {
					return idk.unwrap_unchecked();
				}
			}
			return stn::None;
		}
		stn::Option<chunks::Chunk&> get_chunk(chunks::ChunkLocation pos) {
			return get_chunk_object(pos)
				.map([&](ChunkObject& object)->chunks::Chunk& {return object.get_unchecked<chunks::Chunk>(); });
		}

		stn::Option<chunks::Chunk&> get_chunk(v3::Coord pos) {
			return get_chunk(chunk_from_block_pos(pos));
		}
		stn::Option<ChunkObject&> get_chunk_object(v3::Coord pos) {
			return get_chunk_object(chunk_from_block_pos(pos));
		}
		Option<ecs::Constrained<block>&> get_object(v3::Coord pos) {
			return get_chunk(pos).map([&](chunks::Chunk& chnk)->ecs::Constrained<block>& {return chnk.unchecked_at_pos(pos); });
		}
		Option<block&> get_block(v3::Coord pos) {
			return get_object(pos)
				.map([](chunks::block_object& block_object)->block& {return block_object.get_unchecked<block>(); });
		}
		array<chunks::block_object > voxel_in_range(geo::Box span) {
			array<chunks::block_object > blocks;
			v3::Coord lowest = get_voxel(span.min());
			v3::Coord highest = get_voxel(span.max());
			for (int x = lowest.x; x <= highest.x; x++) {
				for (int y = lowest.y; y <= highest.y; y++) {
					for (int z = lowest.z; z <= highest.z; z++) {
						stn::Option<chunks::block_object&> blk = get_object(Coord(x, y, z));
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
			chunks::ChunkLocation at= chunks::ChunkLocation::from_block_pos(pos);
			if (Chunk.location.position==at.position) {
				return Chunk.unchecked_at_pos(pos).get_unchecked<block>();
			}
			stn::Option<chunks::Chunk&> look = world.get_chunk(at);
			if (!look) {
				return stn::None;
			}
			return look.unwrap_unchecked().unchecked_at_pos(pos).get_unchecked<block>();
		}
		FocusedGridAcessor(ChunkObject::ObjectType chunk_object, grid::Grid& grid) :world(grid), Chunk(chunk_object.get<chunks::Chunk>()) {

		}
		FocusedGridAcessor(ChunkObject::ObjectType chunk_object) :world(Chunk.world().get_resource<grid::Grid>()), Chunk(chunk_object.get<chunks::Chunk>()) {

		}
		Coord get_voxel(v3::Point3 point) {
			return world.get_voxel(point);
		}
	private:
		chunks::Chunk& Chunk;
		grid::Grid& world;

	};
	template<typename T>
	concept WorldAccessor = requires(T & accessor, v3::Coord pos) {
		{
			accessor.get_block(pos)
		}->std::same_as<stn::Option<block&>>;
	};
};

