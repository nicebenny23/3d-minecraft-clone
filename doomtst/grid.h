#include "camera.h"
#include "chunk.h"
#include "vector3.h"
#ifndef grid_HPP
#define grid_HPP
#define loadamt 1
namespace grid {
	
	bool issolidatpos(int x, int y, int z);
	rblock* getobjatgrid(int x, int y, int z);
	void initgrid();
	void load();
	void reupdatechunkborders();
	void initdatabuffer();


};




#endif #pragma once
