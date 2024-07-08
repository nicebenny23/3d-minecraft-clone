#include "camera.h"
#include "chunk.h"
#include "vector3.h"
#ifndef grid_HPP
#define grid_HPP
#define loadamt 1
constexpr auto totalgridsize = (2 * loadamt + 1) * (2 * loadamt + 1) * (2 * loadamt + 1);
namespace grid {
	extern Chunk::chunk** chunklist;
	bool issolidatpos(int x, int y, int z,bool countoob);
	extern v3::Vector3 gridpos;
	block* getobjatgrid(int x, int y, int z,bool countnonsolids =true);
	block* getobjatgrid(v3::Coord pos,bool countnonsolids =true);
	void initgrid();
	void load();
	void reupdatechunkborders();
	void placeblockatloc(int x,int y,int z,int blockid);
	void placeblockatloc(Coord loc, int blockid);
};




#endif #pragma once
