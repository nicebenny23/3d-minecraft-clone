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
	Chunk::chunk** chunklist;
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
	int gridindexfromnormedchunkpos(Coord loc) {

		return gridindexfromnormedchunkpos(loc.x, loc.y, loc.z);
	}

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

	Coord getvoxellocation(Vector3 pos)
	{
		return  Coord(std::floor(pos.x / blocksize), std::floor(pos.y / blocksize), std::floor(pos.z / blocksize));

	}
	block* getvoxel(Vector3 pos)
	{
		return getobjatgrid(getvoxellocation(pos), true);
	}
	

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

	bool issolidatpos(int x, int y, int z, bool countoob)
	{
		Coord chnk = chunkfromblockpos(x, y, z);
		int xchunk = chnk.x;

		int ychunk = chnk.y;
		int zchunk = chnk.z;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{
			//normilized the chunk

			int gridindex = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[gridindex]->blockbuf[Chunk::indexfrompos(x, y, z)];

			//todo seperate for transperent,solid
			if (!blockatpos.attributes.transparent)
			{
				return true;
			}
			return false;
		}
		return countoob;

	}

	//complete
	void initgrid()
	{

		gridpos = camera::campos/ float(chunkaxis);
		griddt = zerov;
		const int size = (2 * loadamt + 1);

		chunklist = new Chunk::chunk * [totalgridsize];
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				for (int k = 0; k < size; k++)
				{
					Coord gridind = Coord(i - loadamt, j - loadamt, k - loadamt);
					chunklist[gridindexfromnormedchunkpos(i, j, k)] = chunkload(gridind);


				}
			}
		}




		int l = 11;
	}

	//order of storage for chunks & location-2loadamnt+1 is width and height
	//z
	//7,8,9   
	//4,5,6
	//1,2,3 x
	//essentially complete

	void grid::load()
	{

		if (!gridchanged())
		{
			return;
		}
		const int size = 2 * loadamt + 1;
		Chunk::chunk** newchunklist = new Chunk::chunk * [totalgridsize];
		int indexdxchange = gridindexfromnormedchunkpos(griddt);

		int ind = 0;
		for (int k = 0; k < size; k++)
		{
			for (int j = 0; j < size; j++) {

				for (int i = 0; i < size; i++) {


					if (chunkloaded(chunklist[ind]->loc))
					{
						//moves to new posi
						newchunklist[ind - indexdxchange] = chunklist[ind];

					}
					else
					{
						chunklist[ind]->destroy();
						int invind = (totalgridsize)-(1 + ind);

						int x = (2 * loadamt - i) + (gridpos.x - loadamt);
						int y = (2 * loadamt - j) + (gridpos.y - loadamt);
						int z = (2 * loadamt - k) + (gridpos.z - loadamt);
						//int y = (2 * loadamt - k) + (gridpos.y - loadamt);
						//gets 
						//	int z = floorabs((invind / static_cast<float>((2 * loadamt + 1)))) - loadamt + gridpos.z;
						newchunklist[invind] =chunkload(Coord(x, y, z));



					}

					ind++;
				}


			}
		}
		delete[] chunklist;

		chunklist = newchunklist;
		//get chunk space every frame
		//(0,0)->(0,0)
		//(16,0)->(1,0)

	}
	Vector3 tovoxelspace(Vector3 point)
	{
		return point / blocksize;
	}
	
	array<block*>& voxelinrange(geometry::Box span)
	{
		array<block*>* blockarr= new array<block*>;
		span.center = tovoxelspace( span.center);
		span.scale= tovoxelspace( span.scale);
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

					blockarr->append(getobjatgrid(x, y, z));
				}

			}

		}
		return *blockarr;
	}
	void reupdatechunkborders()
	{

		Vector3 pos = Vector3(camera::campos);
		pos /= chunklength;
		pos.x = floor(pos.x);
		pos.y = floor(pos.y);
		pos.z = floor(pos.z);
		griddt = pos - gridpos;
		gridpos = pos;
	}
}