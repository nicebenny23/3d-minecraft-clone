#include "../world/grid.h"
#include "../util/queue.h"
using namespace grid;
using namespace queuename;
#ifndef gridmanager_HPP
#define gridmanger_HPP

namespace gridutil{
	extern bool redoallighting;
	void createlightingqueue();
	void computecover(face& blkface);
	void sendrecreatemsg();
void computeallcover();
void emitlight();
void placeblockatloc(int x, int y, int z, int blockid);
void destroyblockatloc(int x, int y, int z);

void redolighting();
     

void setblock(Coord loc, int blockid);
//void removeblockatloc(Coord loc);

}
#endif // !gridmanager_HPP
