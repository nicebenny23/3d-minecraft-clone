#include "block.h"
#ifndef air_HPP
#define air_HPP
inline void airinit(blockname::block* blk) {
	blk->solid = false;
	blk->transparent = true;
	blk->emitedlight = 0;
	blk->id = minecraftair;
	blk->mesh.setfaces(0,0,0,0,0,0);
	blk->mesh.pos = blk->center();
}


#endif // !wood_HPP
