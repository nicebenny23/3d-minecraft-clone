#include "camera.h"
#include "chunk.h"
#include "vector3.h"
#ifndef grid_HPP
#define grid_HPP
#define loadamt 1
constexpr auto totalgridsize = (2 * loadamt + 1) * (2 * loadamt + 1) * (2 * loadamt + 1);
namespace grid {
	extern chunk::chunk** chunklist;
	bool issolidatpos(int x, int y, int z);

	block* getobjatgrid(int x, int y, int z,bool counttransparent =true);
	block* getobjatgrid(v3::Coord pos,bool counttransparent =true);
	void initgrid();
	void load();
	void reupdatechunkborders();
	

};




#endif #pragma once
