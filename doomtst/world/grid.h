
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
		bool contains_chunk_location_when_loaded(chunks::ChunkLocation loc) const {
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
		//returns if it actually contains the chunk
		bool contains_chunk(const chunks::ChunkLocation& pos) {
			return get_chunk(pos).is_some();
		}
		Option<ecs::Constrained<block>&> get_object(v3::Coord pos) {
			return get_chunk(pos).map([&](chunks::Chunk& chnk)->ecs::Constrained<block>& {return chnk.unchecked_at_pos(pos); });
		}
		Option<block&> get_block(v3::Coord pos) {
			return get_object(pos)
				.map([](chunks::block_object& block_object)->block& {return block_object.get_unchecked<block>(); });
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
		FocusedGridAcessor(ChunkObject::ObjectType chunk_object) :world(chunk_object.world().get_resource<grid::Grid>()), Chunk(chunk_object.get<chunks::Chunk>()) {

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

