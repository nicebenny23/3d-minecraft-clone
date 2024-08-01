#include "grid.h"
#include <cmath>
#include "../util/mathutil.h"
#include "../util/vector2.h"
#include "chunkload.h"	
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
	int gridindexfromnormedchunkpos(int xchunk, int ychunk, int zchunk) {

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



	Chunk::chunk* chunkatpos(int x, int y, int z)
	{
		int xchunk = x >> 4;
		int zchunk = z >> 4;
		int ychunk = y >> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{

			int ind = gridindfromchunkpos(xchunk, ychunk, zchunk);
			return chunklist[ind];
		}
		return nullptr;
	}

	void runblockupdates()
	{
		for (int gridind = 0; gridind < totalgridsize; gridind++)
		{
			for (int chunkind = 0; chunkind < chunksize; chunkind++)
			{

				block& blk = (*chunklist[gridind])[chunkind];
				if (blk.id == minecraftair) {

					continue;
				}
				if (blk.id==minecrafttreestone)
				{
					continue;
				}if (blk.id == minecraftstone)
				{
					continue;
				}
				if (blk.id == minecraftmoss)
				{
					continue;
				}
				if (blk.id==minecraftobsidian)
				{
					continue;
				}
				if (gameobject::shouldbeupdated(&blk))
				{

					for (int compind = 0; compind < blk.complist.length; compind++)
					{

						if (blk.complist[compind]->active)
						{

							blk.complist[compind]->update();

						}
					}
				}
			}
		}
	}


	Coord prevgridpos()
	{
		return gridpos - griddt;
	}

	block* getobjatgrid(int x, int y, int z, bool countnonsolids)
	{

		int xchunk = x >> 4;
		int zchunk = z >> 4;
		int ychunk = y >> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{

			int ind = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[ind]->blockbuf[Chunk::indexfrompos(x, y, z)];
			if (countnonsolids ||( blockatpos.id != minecraftair))
			{
				return &blockatpos;
			}

		}
		return nullptr;

	}


	block* getobjatgrid(v3::Coord pos, bool counttransparent)
	{
		return getobjatgrid(pos.x, pos.y, pos.z, counttransparent);
	}

	bool issolidatpos(int x, int y, int z, bool countoob)
	{
		int xchunk = x >> 4;
		int zchunk = z >> 4;
		int ychunk = y >> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{
			//normilized the chunk

			int gridindex = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[gridindex]->blockbuf[Chunk::indexfrompos(x, y, z)];

			//todo seperate for transperent,solid
			if (!blockatpos.transparent)
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

		gridpos = camera::campos/16.f;
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
	void reupdatechunkborders()
	{

		Vector3 pos = Vector3(camera::campos);
		pos /= 16;
		pos.x = floor(pos.x);
		pos.y = floor(pos.y);
		pos.z = floor(pos.z);
		griddt = pos - gridpos;
		gridpos = pos;
	}
}