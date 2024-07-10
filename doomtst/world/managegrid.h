#include "../world/grid.h"
#include "../util/queue.h"
using namespace grid;
using namespace queuename;
#ifndef gridmanager_HPP
#define gridmanger_HPP

namespace gridutil{
	
	void createlightingqueue();
	void computecover(face& blkface);
	void sendrecreatemsg();
void computeallcover();
void emitlight();
void placeblockatloc(int x, int y, int z, int blockid);
void computelightingqueue();
void redolighting();
     
void placeblockatloc(Coord loc, int blockid);
//void removeblockatloc(Coord loc);

}
#endif // !gridmanager_HPP
