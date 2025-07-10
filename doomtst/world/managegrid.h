#include "../world/grid.h"
#include "../util/queue.h"
using namespace grid;
using namespace Cont;
#pragma once 
namespace gridutil {
	extern bool redoallighting;
	void createlightingqueue();
	void computecover(face& blkface);
	void sendrecreatemsg();
	void computeallcover();
	void computepartialcover();
	void emitlight();

	void redolighting();
	
	bool move(block* blk, Coord offset);
	void gridupdate();
	void setblock(Coord loc, int blockid);
	//void removeblockatloc(Coord loc);

}
 // !gridmanager_HPP
