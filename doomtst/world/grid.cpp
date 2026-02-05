#pragma once
#include "grid.h"
#include <cmath>
#include "../math/mathutil.h"
#include "../math/vector2.h"
#include "chunkload.h"	
#include "../math/geometry.h"
#include "../game/GameContext.h"
#include "../util/random.h"
using namespace v3;

namespace grid {



	//normed pos is when pos is in the range [0...2*CtxName::cxt.Grid->length+1) for each direction;
	//scales to block coords
	Point3 Grid::toBlockPos(Point3 point) {
		return Point3(point.x / blocksize, point.y / blocksize, point.z / blocksize);
	}
	template<size_t axis>
	constexpr size_t powof2() requires (axis != 0) && ((axis& (axis - 1)) == 0) {
		size_t newnum = axis;
		int shifts = 0;
		while (newnum > 1) {
			shifts++;
			newnum /= 2;
		}
		return shifts;
	}

	constexpr int shift = powof2< Chunk::chunkaxis>();
	Coord Grid::chunkfromblockpos(Coord pos) {
		return  Coord(pos.x >> shift, pos.y >> shift, pos.z >> shift);
	}


	//gets the location of the currentvoxel
	Coord Grid::getVoxel(Point3 pos) {

		return  Coord(std::floor(pos.x / blocksize), std::floor(pos.y / blocksize), std::floor(pos.z / blocksize));

	}

	Coord Grid::get_chunk(Point3 pos) {
		return  Coord(std::floor(pos.x / Chunk::chunkaxis), std::floor(pos.y / Chunk::chunkaxis), std::floor(pos.z / Chunk::chunkaxis));
	}

	//
	int Grid::localChunkIndex(Coord NormedChunk) {

		return NormedChunk.x + NormedChunk.y * dim_axis + NormedChunk.z * dim_axis * dim_axis;

	}
	int Grid::chunkIndex(Chunk::ChunkLocation pos) {
		return localChunkIndex(pos.position + Coord(rad, rad, rad) - grid_pos.position);
	}
	int Grid::chunkIndex(Coord Chunk) {
		return localChunkIndex(Chunk + Coord(rad, rad, rad) - grid_pos.position);
	}

	//gets the voxel at a place in world space

	bool Grid::chunk_loaded(Coord loc) {
		return(GetChunk(loc) != nullptr);
	}

	bool Grid::contains_chunk_location(Chunk::ChunkLocation loc) const {
		loc.position -= grid_pos.position;
		return (abs(loc.position.x) <= rad && abs(loc.position.y) <= rad && abs(loc.position.z) <= rad);
	}

	bool Grid::contains_chunk_location(Coord loc) const {
		loc -= grid_pos.position;
		return (abs(loc.x) <= rad && abs(loc.y) <= rad && abs(loc.z) <= rad);

	}



	Chunk::chunk* Grid::GetChunk(Coord pos) {
		Coord chnk = chunkfromblockpos(pos);

		if (contains_chunk_location(chnk)) {
			return chunklist.unchecked_at(chunkIndex(chnk));
		}
		return nullptr;
	}
	stn::Option<Chunk::chunk&> Grid::get_chunk(Chunk::ChunkLocation pos) {
		if (contains_chunk_location(pos)) {
			Chunk::chunk* cnk_ptr = chunklist.unchecked_at(chunkIndex(pos));
			if (cnk_ptr) {
				return *cnk_ptr;
			}
		}
		return stn::None;
	}

	stn::Option<Chunk::chunk&> Grid::get_chunk(Coord pos) {
		Coord chnk = chunkfromblockpos(pos);

		if (contains_chunk_location(chnk)) {
			Chunk::chunk* cnk_ptr = chunklist.unchecked_at(chunkIndex(chnk));
			if (cnk_ptr) {
				return *cnk_ptr;
			}
		}
		return stn::None;
	}


	Option<ecs::obj&> Grid::get_object(const v3::Coord pos) {
		return get_chunk(pos).map([&](Chunk::chunk& chnk)->ecs::obj& {return chnk.block_list.unchecked_at(Chunk::index_from_pos(pos)); });
	}


	Option<block&> Grid::get_block(const v3::Coord pos) {
		return get_object(pos).map_member(&ecs::obj::get_component_unchecked<block>);
	}

	block* Grid::getBlock(const v3::Coord pos) {

		ecs::obj* blk = getObject(pos);
		if (blk) {
			return &blk->get_component_unchecked<block>();
		}
		return nullptr;
	}


	ecs::obj* Grid::getObject(const v3::Coord pos) {

		Chunk::chunk* chnk = GetChunk(pos);
		if (chnk) {
			return &chnk->block_list.unchecked_at(Chunk::index_from_pos(pos));
		}
		return nullptr;

	}


	array<block*> Grid::voxelinrange(geo::Box span) {
		array<block*> blockarr = array<block*>();

		v3::Coord lowest = getVoxel(span.min());
		v3::Coord highest = getVoxel(span.max());

		for (int x = lowest.x; x <= highest.x; x++) {
			for (int y = lowest.y; y <= highest.y; y++) {
				for (int z = lowest.z; z <= highest.z; z++) {
					block* blk = getBlock(Coord(x, y, z));
					if (blk != nullptr) {
						blockarr.push(blk);
					}
				}

			}

		}
		return blockarr;
	}

	//order of storage for chunks & location-2loadamnt+1 is width and height
	//z
	//7,8,9   
	//4,5,6
	//1,2,3 x
	void Grid::load(ecs::Ecs& world) {
		stn::array<Chunk::chunk*> newchunklist = stn::array<Chunk::chunk*>(totalChunks, nullptr);

		for (int ind = 0; ind < totalChunks; ind++) {
			bool chunk_loaded = chunklist[ind] != nullptr;
			//sadley need the 2 things in an and to clear it up
			if (chunk_loaded) {
				if (contains_chunk_location(chunklist[ind]->location)) {
					newchunklist[chunkIndex(chunklist[ind]->location)] = chunklist[ind];
				}
				else {
					chunklist[ind]->destroy();
				}
			}
			else {

			}
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
			ecs::obj chunk_object = ecs::spawn_emplaced<Chunk::CreateChunk>(world, spawn_location);
			newchunklist[chunkIndex(spawn_location)] = &chunk_object.get_component<Chunk::chunk>();
		}

		chunklist = std::move(newchunklist);
	}

	void	Grid::updateborders() {
		Point3 pos = camera::campos();
		pos = get_chunk(pos);
		grid_pos = Chunk::ChunkLocation(Coord(pos));
	}
	void Grid::destroy() {

		for (int i = 0; i < totalChunks; i++) {
			chunklist[i]->destroy();
		}

	}
	Grid::Grid(size_t axis) :rad(axis), dim_axis(2 * axis + 1), grid_pos(v3::ZeroCoord) {
		totalChunks = dim_axis * dim_axis * dim_axis;
		chunklist = stn::array<Chunk::chunk*>(totalChunks, nullptr);
	}
}