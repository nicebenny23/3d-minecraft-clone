#include "camera.h"
#include "chunk.h"
#include "vector3.h"
#ifndef grid_HPP
#define grid_HPP
#define loadamt 1
namespace grid {
	extern chunk::chunk** chunklist;
	bool issolidatpos(int x, int y, int z);
	block* getobjatgrid(int x, int y, int z);
	block* getobjatgrid2(int x, int y, int z);
	void initgrid();
	void load();
	void reupdatechunkborders();
	void initdatabuffer();


};




#endif #pragma once
