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
	Vec3 Grid::toBlockPos(Vec3 point)
	{
		return point / blocksize;
	}

	constexpr int powof2(int num) {
		float newnum = num;
		int shifts = 0;
		while (newnum > 1)
		{
			shifts++;
			newnum /= 2;
		}
		if (newnum == 1)
		{
			return shifts;
		}
		static_assert("chunkaxis msut be a power of 2");
		return -1;
	}

	constexpr int shift = powof2(chunkaxis);
	Coord Grid::chunkfromblockpos(Coord pos)
	{
		pos.x= pos.x >> shift;
		pos.y= pos.y >> shift;
		pos.z=pos.z >> shift;
		return pos;
	}


	//gets the location of the currentvoxel
	Coord Grid::getVoxel(Vec3 pos)
	{
		return  Coord(std::floor(pos.x / blocksize), std::floor(pos.y / blocksize), std::floor(pos.z / blocksize));

	}
	
	//
	int Grid::localChunkIndex(Coord NormedChunk)
	{

		return NormedChunk.x + NormedChunk.y* (dim()) + NormedChunk.z* dim()*dim();
	}
	int Grid::chunkIndex(Coord Chunk)
	{
		Chunk+= Coord(rad, rad, rad) - gridpos;
		return localChunkIndex(Chunk);
	}
	
	//gets the voxel at a place in world space

	bool Grid::ChunkLoaded(Coord loc)
	{
		return(GetChunk(loc) != nullptr);
	}

	bool Grid::containsChunk(Coord loc)
	{
		loc -= gridpos;
		return (abs(loc.x) <= rad && abs(loc.y) <= rad && abs(loc.z) <= rad);

	}



	Chunk::chunk* Grid::GetChunk(Coord pos)
	{
		Coord chnk = chunkfromblockpos(pos);

		if (containsChunk(chnk))
		{

			return chunklist[chunkIndex(chnk)];
			
		}
		return nullptr;
	} 

	block* Grid::getBlock(const v3::Coord pos)
	{

		gameobject::obj* blk = getObject(pos);
		if (blk)
		{
			return &blk->getcomponent<block>();
		}
		return nullptr;
	}

	gameobject::obj* Grid::getObject(const v3::Coord pos)
	{
		Coord chunk_pos = chunkfromblockpos(pos);
		if (containsChunk(chunk_pos))
		{


			Chunk::chunk* chnk = chunklist[chunkIndex(chunk_pos)];
			if (chnk != nullptr)
			{

				return &(*chnk)[Chunk::indexfrompos(pos)];


			}
		}
		return nullptr;

	}


	array<block*>& Grid::voxelinrange(geometry::Box span)
	{
		array<block*>* blockarr = new array<block*>;
		span.center = toBlockPos(span.center);
		span.scale = toBlockPos(span.scale);
		v3::Vec3 lowpos = ((span.center - span.scale) - unitv);

		v3::Coord lowest = v3::Coord(symmetric_floor(lowpos.x), symmetric_floor(lowpos.y), symmetric_floor(lowpos.z));
		v3::Vec3 highpos = ((span.center + span.scale) + unitv);

		v3::Coord highest = v3::Coord(symmetric_ceil(highpos.x), symmetric_ceil(highpos.y), symmetric_ceil(highpos.z));

		for (int x = lowest.x; x < highest.x; x++)
		{
			for (int y = lowest.y; y < highest.y; y++)
			{
				for (int z = lowest.z; z < highest.z; z++)
				{
					block* blk = getBlock(Coord(x, y, z));
					if (blk != nullptr)
					{

						blockarr->push(blk);
					}
				}

			}

		}
		return *blockarr;
	}

	//order of storage for chunks & location-2loadamnt+1 is width and height
	//z
	//7,8,9   
	//4,5,6
	//1,2,3 x
	void Grid::load()
	{

	
		stn::array<Chunk::chunk*>	newchunklist = stn::array<Chunk::chunk*>(totalChunks, nullptr);
		int indexdxchange = localChunkIndex(griddt);
		
		size_t ind = 0;
		for (int k = 0; k < dim(); k++)
		{
			for (int j = 0; j < dim(); j++) {

				for (int i = 0; i < dim(); i++) {

					//we  need to check if is null because we call it with load,with a bunch of unitilized chunks
					bool ChunkLoaded = chunklist[ind] != nullptr;
					//sadley need the 2 things in an and to clear it up
					if (ChunkLoaded && containsChunk(chunklist[ind]->loc))
					{
						newchunklist[ind - indexdxchange] = chunklist[ind];

					}
					else
					{
						if (ChunkLoaded)
						{

							chunklist[ind]->destroy();

						}
					}
					
					ind++;
				}


			}
		}
		has_loaded_chunk = false;
	 
		ind = 0;
		for (int k = 0; k < dim(); k++)
		{
			for (int j = 0; j < dim(); j++) {

				for (int i = 0; i < dim(); i++) {

					if (newchunklist[ind] != nullptr)
					{

						ind++;
						continue;
					}
					//the inverse ind of 0 in an array of 9 is  8 ,1 is 7 follows formula totalinds-inds-1
					
					int x = (i) + (gridpos.x - rad);
					int y = (j) + (gridpos.y - rad);
					int z = (k) + (gridpos.z - rad);
					if (!has_loaded_chunk)
					{
						
						newchunklist[ind] = loader.LoadChunk(Coord(x, y, z));
						
						has_loaded_chunk = true;
					}



					ind++;
					
				}
			}
		
		}
		chunklist = std::move(newchunklist);
	}

	void	Grid::updateborders()
	{
		Vec3 pos = camera::campos();
		pos /= chunklength;
		pos.x = floor(pos.x);
		pos.y = floor(pos.y);
		pos.z = floor(pos.z);
		griddt = Coord(pos) - gridpos;
		gridpos = Coord(pos);
	}
	void Grid::destroy()
	{

		for (int i = 0; i<totalChunks; i++) {
			chunklist[i]->destroy();
		}
		chunklist.destroy();
		
	}
	Grid::Grid(int axis,CtxName::Context* Context):rad(axis), totalChunks(dim()*dim()*dim())
	{
		ctx = Context;
		Context->Grid = this;
		loader.Init(ctx);
		gridpos =Coord( camera::campos() / float(chunkaxis));
		griddt = zeroiv;
		chunklist = stn::array<Chunk::chunk*>(totalChunks,nullptr);
	
		load();
	}
	bool Grid::haschanged() {
		return(griddt != zeroiv)||has_loaded_chunk;
	}
}