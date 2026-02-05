#include "../block/block.h"
#include "../util/cached.h"
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
	std::string getcorefilename(ChunkLocation pos);
	inline size_t index_from_pos(Coord pos) {
		int x = symmetric_mod(pos.x, chunkaxis);
		int y = symmetric_mod(pos.y, chunkaxis);
		int z = symmetric_mod(pos.z, chunkaxis);
		return	chunkaxis * chunkaxis * x + chunkaxis * y + z;
	}
	struct chunk:ecs::component
	{
		Chunk::ChunkLocation location;
		bool modified;
		void write();
		chunk(Chunk::ChunkLocation chunk_location) :modified(false), location(chunk_location), block_list(chunksize) {

		}
		Point3 center() const {
			return location.center();
		}
		geo::Box span() const {
			return location.span();
		}
		ecs::obj& operator[](size_t index) {
			return block_list[index];
		}
		const ecs::obj& operator[](size_t index) const{
			return block_list[index];
		}
		stn::array<ecs::obj> block_list;

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
		void destroy();
	};

}

 // !Chunk_H
#pragma once
