#include "block.h"
#ifndef air_HPP
#define air_HPP
inline void airinit(blockname::block* blk) {
	blk->attributes.solid = false;
	blk->attributes.transparent = true;
	blk->emitedlight = 0;
	blk->id = minecraftair;
	blk->mesh.setfaces(0,0,0,0,0,0);
	blk->mesh.pos = blk->center();

	
blk->minedfastwithpick = false;
	blk->mininglevel = 0;
	blk->lightval = 0;
	blk->bstate.broken = false;
}


#endif // !wood_HPP
