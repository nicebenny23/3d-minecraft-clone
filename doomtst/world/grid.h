#include "../game/camera.h"
#include "chunkload.h"
#include "../util/vector3.h"
#include "../util/geometry.h"
#ifndef grid_HPP
#define grid_HPP
#define loadamt 1

constexpr auto totalgridsize = (2 * loadamt + 1) * (2 * loadamt + 1) * (2 * loadamt + 1);
namespace grid {
	bool gridchanged();
	extern dynamicarray::array<Chunk::chunk*> chunklist;
	extern v3::Vector3 gridpos;
	Coord prevgridpos();
	block* getobjatgrid(const int x,const int y,const int z, const bool countnonsolids = true);
	block* getobjatgrid(const v3::Coord pos, const bool countnonsolids = true);
	void initgrid();
	void load();
	Vector3 scaletoblocksize(Vector3 point);
	Coord chunkfromblockpos(int x,int y,int z);
	array<block*>& voxelinrange(geometry::Box span);
	void updatechunkborders();
	Chunk::chunk* chunkatpos(int x, int y, int z);

	Chunk::chunk* chunkatpos(int x, int y, int z);
	 Coord   getvoxellocation(Vector3 pos);
	
};


#endif #pragma once
