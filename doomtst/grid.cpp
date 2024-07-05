#include "grid.h"
#include <cmath>
#include "vector2.h"
using namespace v3;

namespace grid {
	inline int modabs(int x, int m) {

		if (x < 0)
		{
			if (static_cast<double>(x) / m == int(x / m))
			{
				//so (-16,-16) is now a part of the correct chunk this has to bedon due to the inverse taken
				return 0;
			}
			else {
				return m - ((-x) % m);
			}
		}
		else
		{
			return x % m;
		}
	}
	
	float floorabs(float x) {

		if (x < 0)
		{
			return -1 * floor(-1 * x);
		}
		else
		{
			return floor(x);
		}
	}



	Vector3 gridpos;
	Vector3 griddt;
	chunk::chunk** chunklist;
	bool chunkloaded(int xchunk, int ychunk, int zchunk) {
		if (abs(xchunk - gridpos.x) <= loadamt && abs(ychunk - gridpos.y) <= loadamt && abs(zchunk - gridpos.z) <= loadamt) {

			return true;
		}
		return false;
	}
	bool chunkloaded(Coord loc) {
		if (abs(loc.x - gridpos.x) <= loadamt && abs(loc.y - gridpos.y) <= loadamt && abs(loc.z - gridpos.z) <= loadamt) {

			return true;
		}
		return false;
	}
	bool normedchunkloaded(Coord loc) {
		if (abs(loc.x) <= loadamt && abs(loc.y) <= loadamt && abs(loc.z) <= loadamt) {

			return true;
		}
		return false;
	}
	int gridindfromnormedpos(int xchunk, int ychunk, int zchunk) {

		return xchunk + ychunk * (2 * loadamt + 1) + zchunk * (2 * loadamt + 1) * (2 * loadamt + 1);
	}
	int gridindfromchunkpos(int xchunk, int ychunk, int zchunk) {
		xchunk += loadamt - gridpos.x;
		zchunk += loadamt - gridpos.z;
		ychunk += loadamt - gridpos.y;
		return gridindfromnormedpos(xchunk, ychunk, zchunk);
	}
	

	


	block* getobjatgrid(int x, int y, int z,bool counttransparent)
	{

		int xchunk = x >> 4;
		int zchunk = z >> 4;
		int ychunk = y >> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{
			//normilized the chunk

			int ind = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[ind]->blockstruct[chunk::indexfrompos(x, y, z)];
			if (counttransparent || blockatpos.id != minecraftair)
			{
				return &blockatpos;
			}

		}
		return nullptr;

	}
	

	block* getobjatgrid(v3::Coord pos, bool counttransparent)
	{
		int xchunk = pos.x >> 4;
		int zchunk = pos.z >> 4;
		int ychunk = pos.y>> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{
			//normilized the chunk

			int ind = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[ind]->blockstruct[chunk::indexfrompos(pos.x, pos.y, pos.z)];
			if (counttransparent || blockatpos.id != minecraftair)
			{
				return &blockatpos;
			}

		}
		return nullptr;

	}

	bool issolidatpos(int x, int y, int z)
	{
		int xchunk = x >> 4;
		int zchunk = z >> 4;
		int ychunk = y >> 4;
		if (chunkloaded(xchunk, ychunk, zchunk))
		{
			//normilized the chunk

			int gridindex = gridindfromchunkpos(xchunk, ychunk, zchunk);
			block& blockatpos = chunklist[gridindex]->blockstruct[chunk::indexfrompos(x, y, z)];
			if (blockatpos.id != minecraftair&& blockatpos.id != minecraftwater)
			{
				return true;
			}

		}
		return false;

	}
	//complete
	void initgrid()
	{
		
		gridpos = zerov;
		griddt = zerov;
		const int size = (2 * loadamt + 1);
		
		chunklist = new chunk::chunk * [totalgridsize];
		for (int i = 0; i <size; i++)
		{
			for (int j = 0; j < size;j++)
			{
			   for (int k =0; k <size;k++)
			    {
				   Coord gridind = Coord(i - loadamt, j - loadamt, k - loadamt);
					chunklist[gridindfromnormedpos(i,j,k)] = chunk::load( gridind);

				}
			}
		}
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
		
		chunk::chunk** newchunklist = new chunk::chunk * [size * size*size];
		int indexdxchange = gridindfromnormedpos(griddt.x, griddt.y, griddt.z);

		for (int i = 0; i < size; i++)
		{
			for (int j = 0;j < size;j++) {

				for (int k = 0;k < size;k++) {

					int ind = gridindfromnormedpos(i, j,k);

					if (chunkloaded(chunklist[ind]->loc))
					{
						//moves to new posi
						newchunklist[ind - indexdxchange] = chunklist[ind];

					}
					else
					{
						
						int invind = (totalgridsize) - (1 + ind);

						int x = (2 * loadamt - i) + (gridpos.x - loadamt);
						int y = (2 * loadamt - j) + (gridpos.y - loadamt);
						int z = (2 * loadamt - k) + (gridpos.z - loadamt);
						//int y = (2 * loadamt - k) + (gridpos.y - loadamt);
						//gets 
						//	int z = floorabs((invind / static_cast<float>((2 * loadamt + 1)))) - loadamt + gridpos.z;
						newchunklist[invind] = chunk::load(Coord(x,y,z));

						chunklist[ind]->destroy();

					}


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