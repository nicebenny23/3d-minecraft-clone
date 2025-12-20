#pragma once
#include "grid.h"
#include <cmath>
#include "../util/mathutil.h"
#include "../util/vector2.h"
#include "chunkload.h"	
#include "../util/geometry.h"
#include "../game/GameContext.h"
#include "../util/random.h"
using namespace v3;

namespace grid {



	//normed pos is when pos is in the range [0...2*CtxName::cxt.Grid->length+1) for each direction;
	//scales to block coords
	Point3 Grid::toBlockPos(Point3 point) {
		return Point3(point.x / blocksize, point.y / blocksize, point.z / blocksize);
	}

	constexpr int powof2(int num) {
		float newnum = num;
		int shifts = 0;
		while (newnum > 1) {
			shifts++;
			newnum /= 2;
		}
		if (newnum == 1) {
			return shifts;
		}
		static_assert("chunkaxis msut be a power of 2");
		return -1;
	}

	constexpr int shift = powof2(chunkaxis);
	Coord Grid::chunkfromblockpos(Coord pos) {
		return  Coord(pos.x >> shift, pos.y >> shift, pos.z >> shift);
	}


	//gets the location of the currentvoxel
	Coord Grid::getVoxel(Point3 pos) {

		return  Coord(std::floor(pos.x / blocksize), std::floor(pos.y / blocksize), std::floor(pos.z / blocksize));

	}

	Coord Grid::get_chunk(Point3 pos) {
		return  Coord(std::floor(pos.x / chunkaxis), std::floor(pos.y / chunkaxis), std::floor(pos.z / chunkaxis));
	}

	//
	int Grid::localChunkIndex(Coord NormedChunk) {

		return NormedChunk.x + NormedChunk.y * dim_axis + NormedChunk.z * dim_axis * dim_axis;

	}
	int Grid::chunkIndex(Coord Chunk) {
		return localChunkIndex(Chunk + Coord(rad, rad, rad)-gridpos);
	}

	//gets the voxel at a place in world space

	bool Grid::ChunkLoaded(Coord loc) {
		return(GetChunk(loc) != nullptr);
	}

	bool Grid::containsChunk(Coord loc) const {
		loc -= gridpos;
		return (abs(loc.x) <= rad && abs(loc.y) <= rad && abs(loc.z) <= rad);

	}



	Chunk::chunk* Grid::GetChunk(Coord pos) {
		Coord chnk = chunkfromblockpos(pos);

		if (containsChunk(chnk)) {
			return chunklist.unchecked_at(chunkIndex(chnk));
		}
		return nullptr;
	}
	stn::Option<Chunk::chunk&> Grid::get_chunk(Coord pos) {
		Coord chnk = chunkfromblockpos(pos);

		if (containsChunk(chnk)) {
			Chunk::chunk* chunk = chunklist.unchecked_at(chunkIndex(chnk));
			if (chunk) {
				return stn::Option<Chunk::chunk&>(*chunk);
			}
		}
		return None;
	}


	Option<ecs::obj&> Grid::get_object(const v3::Coord pos) {
		Coord chunk_pos = chunkfromblockpos(pos);
		int chunk_index = chunkIndex(chunk_pos);
		//since if it was le than zero it would fit
		if (containsChunkIndex(chunk_index)) {
			Chunk::chunk* chnk = chunklist.unchecked_at(static_cast<size_t>(chunk_index));
			if (chnk != nullptr) {
				return stn::Option<ecs::obj&>(chnk->blockbuf.unchecked_at(Chunk::indexfrompos(pos)));
			}
		}
		return stn::None;
	}


	block* Grid::getBlock(const v3::Coord pos) {

		ecs::obj* blk = getObject(pos);
		if (blk) {
			return &blk->get_component_unchecked<block>();
		}
		return nullptr;
	}

	bool Grid::containsChunkIndex(int index) const {
		return 0 <= index && index < totalChunks;
	}

	ecs::obj* Grid::getObject(const v3::Coord pos) {
		Coord chunk_pos = chunkfromblockpos(pos);
		int chunk_index = chunkIndex(chunk_pos);
		//since if it was le than zero it would fit
		if (containsChunkIndex(chunk_index)) {
			Chunk::chunk* chnk = chunklist.unchecked_at(static_cast<size_t>(chunk_index));
			if (chnk != nullptr) {

				return &chnk->blockbuf.unchecked_at(Chunk::indexfrompos(pos));
			}
		}
		return nullptr;

	}


	array<block*> Grid::voxelinrange(geometry::Box span) {
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
	void Grid::load() {
		stn::array<Chunk::chunk*> newchunklist = stn::array<Chunk::chunk*>(totalChunks, nullptr);
		int indexdxchange = localChunkIndex(griddt);

		for (int ind = 0; ind < totalChunks; ind++) {
			bool ChunkLoaded = chunklist[ind] != nullptr;
			//sadley need the 2 things in an and to clear it up
			if (ChunkLoaded && containsChunk(chunklist[ind]->loc)) {
				newchunklist[ind - indexdxchange] = chunklist[ind];
			}
			else {
				if (ChunkLoaded) {
					chunklist[ind]->destroy();
				}
			}
		}
		has_loaded_chunk = false;

		size_t ind = 0;
		for (int k = 0; k < dim_axis; k++) {
			for (int j = 0; j < dim_axis; j++) {
				for (int i = 0; i < dim_axis; i++) {
					if (newchunklist[ind] == nullptr) {
						if (debug_slow || !has_loaded_chunk) {
							newchunklist[ind] = loader.LoadChunk(Coord(i, j, k) + gridpos - Coord(rad, rad, rad));
							has_loaded_chunk = true;
						}
					}
					ind++;
				}
			}
		}
		chunklist = std::move(newchunklist);
	}

	void	Grid::updateborders() {
		Point3 pos = camera::campos();
		pos = get_chunk(pos);
		griddt = Coord(pos) - gridpos;

		gridpos = Coord(pos);
	}
	void Grid::destroy() {

		for (int i = 0; i < totalChunks; i++) {
			chunklist[i]->destroy();
		}

	}
	Grid::Grid(int axis, CtxName::Context* Context) :rad(axis), dim_axis(2 * axis + 1) {
		totalChunks = dim_axis * dim_axis * dim_axis;
		ctx = Context;
		Context->Grid = this;
		loader.Init(ctx);
		gridpos = ZeroCoord;
		griddt = ZeroCoord;
		chunklist = stn::array<Chunk::chunk*>(totalChunks, nullptr);

		load();
	}
	bool Grid::haschanged() {
		return(griddt != ZeroCoord) || (has_loaded_chunk) || (griddt != ZeroCoord && debug_slow);
	}
}