#include "grid.h"
#include <cmath>
#include "../util/mathutil.h"
#include "../util/vector2.h"
#include "chunkload.h"	
#include "../util/geometry.h"
using namespace v3;

namespace grid {



	Vector3 gridpos;
	Vector3 griddt;

	dynamicarray::array<Chunk::chunk*> grid::chunklist;
	bool gridchanged() {
		return(griddt != zeroiv);
	}
	bool chunkloaded(int xchunk, int ychunk, int zchunk) {
		if (abs(xchunk - gridpos.x) <= loadamt && abs(ychunk - gridpos.y) <= loadamt && abs(zchunk - gridpos.z) <= loadamt) {

			return true;
		}
		return false;
	}
	bool chunkloaded(Coord loc) {
		return chunkloaded(loc.x, loc.y, loc.z);
	}
	//normed pos is when pos is in the range [0...2*loadamt+1) for each direction;
	int gridindexfromnormedchunkpos(const int xchunk, const int ychunk, const int zchunk) {

		return xchunk + ychunk * (2 * loadamt + 1) + zchunk * (2 * loadamt + 1) * (2 * loadamt + 1);
	}
	//normed pos is when pos is in the range [0...2*loadamt+1) for each direction;
	

	int gridindfromchunkpos(int xchunk, int ychunk, int zchunk) {

		xchunk += loadamt - gridpos.x;
		zchunk += loadamt - gridpos.z;
		ychunk += loadamt - gridpos.y;
		return gridindexfromnormedchunkpos(xchunk, ychunk, zchunk);
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
	Coord chunkfromblockpos(const int x, const int y, const int z)
	{
		int xchunk =x>>shift;
		int ychunk = y>> shift;
		int zchunk = z >> shift;
		return Coord(xchunk, ychunk, zchunk);
	}
	

	Chunk::chunk* chunkatpos(int x, int y, int z)
	{
		Coord chnk = chunkfromblockpos(x, y, z);
		int xchunk = chnk.x;

		int ychunk = chnk.y;
		int zchunk = chnk.z;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{

			int ind = gridindfromchunkpos(xchunk, ychunk, zchunk);
			return chunklist[ind];
		}
		return nullptr;
	}
	//gets the location of the currentvoxel
	Coord getvoxellocation(Vector3 pos)
	{
		return  Coord(std::floor(pos.x / blocksize), std::floor(pos.y / blocksize), std::floor(pos.z / blocksize));

	}
	//gets the voxel at a place in world space
	

	Coord prevgridpos()
	{
		return gridpos - griddt;
	}
	
	block* getobjatgrid(const int x, const int y, const int z,const  bool countnonsolids)
	{

		Coord chnk = chunkfromblockpos(x, y, z);
		int xchunk = chnk.x;

		int ychunk = chnk.y;
		int zchunk = chnk.z;
		
		if (chunkloaded(xchunk, ychunk, zchunk))
		{

		const int ind = gridindfromchunkpos(xchunk, ychunk, zchunk);
			int blkind = Chunk::indexfrompos(x, y, z);
			block& blockatpos = chunklist[ind]->blockbuf[blkind];
			if (countnonsolids ||( !blockatpos.attributes.solid))
			{
				return &blockatpos;
			}

		}
		return nullptr;

	}


	block* getobjatgrid(const v3::Coord pos,const  bool counttransparent)
	{
		return getobjatgrid(pos.x, pos.y, pos.z, counttransparent);
	}

	//complete
	void initgrid()
	{

		gridpos = camera::campos/ float(chunkaxis);
		griddt = zerov;
		const int size = (2 * loadamt + 1);

		chunklist = dynamicarray::array<Chunk::chunk*>(totalgridsize);
		for (int i = 0; i < totalgridsize; i++)
		{
			chunklist[i] = nullptr;
		}
		load();



	}

	//order of storage for chunks & location-2loadamnt+1 is width and height
	//z
	//7,8,9   
	//4,5,6
	//1,2,3 x
//todo simplyfy
	void grid::load()
	{

	
		const int size = 2 * loadamt + 1;

		dynamicarray::array<Chunk::chunk*>	newchunklist = dynamicarray::array<Chunk::chunk*>(totalgridsize);
		int indexdxchange = gridindexfromnormedchunkpos(griddt.x,griddt.y,griddt.z);

		int ind = 0;
		for (int k = 0; k < size; k++)
		{
			for (int j = 0; j < size; j++) {

				for (int i = 0; i < size; i++) {

					//we  need to check if is null because we call it with load,with a bunch of unitilized chunks
					bool ischunknull = chunklist[ind] == nullptr;
					if (!ischunknull&&chunkloaded(chunklist[ind]->loc))
					{
						newchunklist[ind - indexdxchange] = chunklist[ind];

					}
					else
					{
						if (!ischunknull)
						{

							chunklist[ind]->destroy();

						}
						//the inverse ind of 0 in an array of 9 is  8 ,1 is 7 follows formula totalinds-inds-1
						int invind = (totalgridsize)-(1 + ind);

						int x = (2 * loadamt - i) + (gridpos.x - loadamt);
						int y = (2 * loadamt - j) + (gridpos.y - loadamt);
						int z = (2 * loadamt - k) + (gridpos.z - loadamt);

						newchunklist[invind] =loadchunk(Coord(x, y, z));



					}

					ind++;
				}


			}
		}
	
		chunklist =std::move( newchunklist);
		//get chunk space every frame
		//(0,0)->(0,0)
		//(16,0)->(1,0)

	}
	Vector3 scaletoblocksize(Vector3 point)
	{
		return point / blocksize;
	}
	
	array<block*>& voxelinrange(geometry::Box span)
	{
		array<block*>* blockarr= new array<block*>;
		span.center = scaletoblocksize( span.center);
		span.scale= scaletoblocksize( span.scale);
		v3::Vector3 lowpos = ((span.center - span.scale) - unitv);

		v3::Coord lowest = v3::Coord(floorabs(lowpos.x), floorabs(lowpos.y), floorabs(lowpos.z));
		v3::Vector3 highpos = ((span.center + span.scale) + unitv);

		v3::Coord highest = v3::Coord(ceilabs(highpos.x), ceilabs(highpos.y), ceilabs(highpos.z));

		for (int x = lowest.x; x < highest.x; x++)
		{
			for (int y = lowest.y; y < highest.y; y++)
			{
				for (int z = lowest.z; z < highest.z; z++)
				{
					block* blk = getobjatgrid(x, y, z);
					if (blk!=nullptr)
					{

						blockarr->append(blk);
					}
				}

			}

		}
		return *blockarr;
	}
	void updatechunkborders()
	{

		Vector3 pos = camera::campos;
		pos /= chunklength;
		pos.x = floor(pos.x);
		pos.y = floor(pos.y);
		pos.z = floor(pos.z);
		griddt = pos - gridpos;
		gridpos = pos;
	}
}