#include "../block/block.h"
#include "../util/cached.h"
#include "../game/ecs/filtered_object.h"
#include "chunk_mesh.h"
#pragma once 
using namespace blocks;
namespace Chunk {

	constexpr size_t chunkaxis = size_t(chunklength / blocksize);
	constexpr size_t chunksize = chunkaxis * chunkaxis * chunkaxis;

	struct ChunkLocation {

		ChunkLocation(v3::Coord pos) :position(pos) {

		}
		bool operator==(const ChunkLocation& other) const = default;
		bool operator!=(const ChunkLocation& other) const = default;

		v3::Coord position;
		Point3 center() const {
			return (position + unitv / 2.f) * chunklength;
		}
		geo::Box span() const {
			return geo::Box(center(), v3::Scale3(chunklength)/2);
		}
		bool contains_block(v3::Coord block_position) const {

		}
		size_t distance_to(const ChunkLocation& c2) const{
			return v3::manhattan_distance(position, c2.position);
		}
	};
	inline std::string file_name(ChunkLocation pos, grid::world& world) {
		std::string m = std::format("Chunk{}", pos.position);
		std::filesystem::path path = world.get_path();
		path = path / "Chunks" / m;  // assign the combined path back
		return 	path.string();
	}
	inline size_t index_from_pos(Coord pos) {
		int x = symmetric_mod(pos.x, chunkaxis);
		int y = symmetric_mod(pos.y, chunkaxis);
		int z = symmetric_mod(pos.z, chunkaxis);
		return	chunkaxis * chunkaxis * x + chunkaxis * y + z;
	}
	struct chunk:ecs::component
	{
		stn::array<ecs::Constrained<block>> block_list;
		Chunk::ChunkLocation location;
		bool modified;
		chunk(Chunk::ChunkLocation chunk_location) :modified(false), location(chunk_location), block_list() {
			block_list.reserve(chunksize);
		}

		Point3 center() const {
			return location.center();
		}
		geo::Box span() const {
			return location.span();
		}
		ecs::Constrained<block>& operator[](size_t index) {
			return block_list[index];
		}
		const ecs::Constrained<block>& operator[](size_t index) const{
			return block_list[index];
		}

		using iterator = decltype(block_list)::iterator;
		using const_iterator = decltype(block_list)::const_iterator;
		iterator begin() {
			return block_list.begin();
		}
		iterator end() {
			return block_list.end();
		}
		const_iterator begin() const{
			return block_list.begin();
		}
		const_iterator end() const{
			return block_list.end();
		}
		
		void destroy_hook() {
			if (modified) {
				file_handle file = file_handle(file_name(location.position,world().get_resource<grid::world>()), FileMode::ReadWriteBinary);
				for (int i = 0; i < chunksize; i++) {
					block& block_at = block_list[i].get<block>();
					stn::file_serializer<block_id>().write(block_at.id, file);
					stn::file_serializer<math::Direction2d>().write(block_at.mesh.direction, file);
					stn::file_serializer<math::Direction3d>().write(block_at.mesh.attached_direction, file);
					block_at.type()->write_into_bytes(block_list[i].object(), file);
				}
				file.close();
			}
			for (int i = 0; i < chunksize; i++) {
				std::move(block_list[i]).destroy();
			}
		}
	};

}

 // !Chunk_H
#pragma once
