#include "../game/camera.h"
#include "chunkload.h"
#include "../util/vector3.h"
#ifndef grid_HPP
#define grid_HPP
#define loadamt 1
constexpr auto totalgridsize = (2 * loadamt + 1) * (2 * loadamt + 1) * (2 * loadamt + 1);
namespace grid {
	bool gridchanged();
	extern Chunk::chunk** chunklist;
	bool issolidatpos(int x, int y, int z, bool countoob);
	extern v3::Vector3 gridpos;
	Coord prevgridpos();
	block* getobjatgrid(int x, int y, int z, bool countnonsolids = true);
	block* getobjatgrid(v3::Coord pos, bool countnonsolids = true);
	void initgrid();
	void load();
	void reupdatechunkborders();
	Chunk::chunk* chunkatpos(int x, int y, int z);
	void runblockupdates();
	Chunk::chunk* chunkatpos(int x, int y, int z);
};


#endif #pragma once
