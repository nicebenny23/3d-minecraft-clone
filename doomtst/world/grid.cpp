#include "grid.h"
#include <cmath>
#include "../util/mathutil.h"
#include "../util/vector2.h"
using namespace v3;

namespace grid {



	Vector3 gridpos;
	Vector3 griddt;
	Chunk::chunk** chunklist;
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





	block* getobjatgrid(int x, int y, int z, bool countnonsolids)
	{

		int xchunk = x >> 4;
		int zchunk = z >> 4;
		int ychunk = y >> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{

			int ind = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[ind]->blockstruct[Chunk::indexfrompos(x, y, z)];
			if (countnonsolids || blockatpos.id != minecraftair)
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

	bool issolidatpos(int x, int y, int z,bool countoob)
	{
		int xchunk = x >> 4;
		int zchunk = z >> 4;
		int ychunk = y >> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{
			//normilized the chunk

			int gridindex = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[gridindex]->blockstruct[Chunk::indexfrompos(x, y, z)];

			//todo seperate for transperent,solid
			if (!blockatpos.transparent)
			{
				return true;
			}
			return false;
		}
		return countoob;

	}
	void computecover(face* blkface) {
		// Ensure blkface is not null
	

	
		
		if (blkface->mesh->blk->pos!=zeroiv)
		{
			Coord pos = blkface->mesh->blk->pos + dirfromint(blkface->facenum);
			Vector3 a = blkface->mesh->blk->pos;
			block* blk = getobjatgrid(pos, true);
			if (blk != nullptr)
			{
				if (blk->transparent)
				{


					if (blk->id != blkface->mesh->blk->id)
					{
						blkface->covered = false;
					}
				}

			}
			else
			{
				blkface->covered = false;
			}
		}
		else {
			Coord pos = blkface->mesh->blk->pos + dirfromint(blkface->facenum);
			//std::cout << pos.x<<'\n';
			blkface->covered = issolidatpos(pos.x, pos.y, pos.z, true);
		}
	
	}
	void sendrecreatemsg() {

		for (int i = 0; i < totalgridsize; i++)
		{
			chunklist[i]->mesh->meshrecreateneeded=true;
		}
	}
	void computeallcover() {

		for (int gridind = 0; gridind < totalgridsize; gridind++)
		{
			for (int blockind = 0;blockind < chunksize; blockind++) {
				for (int faceind = 0; faceind < 6; faceind++)
				{
					face& tocover = (*(chunklist[gridind]->blockstruct[blockind]).mesh)[faceind];
					computecover(&tocover);
				}

			}
		}
		sendrecreatemsg();
	}
	void placeblockatloc(int x, int y, int z, int blockid)
	{
		block* location = getobjatgrid(x, y, z);
		if (location != nullptr)
		{

			setair(location);
			location->id = blockid;
			giveblocktraits(location);
			for (int faceind = 0; faceind < 6; faceind++)
			{
				computecover(&((*(*location).mesh)[faceind]));
			}
			for (int blkind = 0; blkind < 6; blkind++)
			{
				block* blockatpos = getobjatgrid(dirfromint(blkind) + location->pos);
				for (int faceind = 0;faceind < 6;faceind++) {
					computecover(&(*(*blockatpos).mesh)[faceind]);

				}
			}
		}
		sendrecreatemsg();
	}

	void placeblockatloc(Coord loc, int blockid)
	{
		placeblockatloc(loc.x, loc.y, loc.z, blockid);
	}

	//complete
	void initgrid()
	{

		gridpos = zerov;
		griddt = zerov;
		const int size = (2 * loadamt + 1);

		chunklist = new Chunk::chunk * [totalgridsize];
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size;j++)
			{
				for (int k = 0; k < size;k++)
				{
					Coord gridind = Coord(i - loadamt, j - loadamt, k - loadamt);
					chunklist[gridindexfromnormedchunkpos(i, j, k)] = Chunk::load(gridind);

				}
			}
		}
		computeallcover();
	
	}

	//order of storage for chunks & location-2loadamnt+1 is width and height
	//z
	//7,8,9   
	//4,5,6
	//1,2,3 x
	//essentially complete

	void grid::load()
	{
		const int size = 2 * loadamt + 1;

		Chunk::chunk** newchunklist = new Chunk::chunk * [totalgridsize];
		int indexdxchange = gridindexfromnormedchunkpos(griddt);

		int ind = 0;
		for (int k = 0; k < size; k++)
		{
			for (int j = 0;j < size;j++) {

				for (int i = 0;i < size;i++) {


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
						newchunklist[invind] =Chunk::load(Coord(x, y, z));



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
		if (griddt != zeroiv)
		{
			computeallcover();
		}
		
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