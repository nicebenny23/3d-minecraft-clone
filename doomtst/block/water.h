#include "block.h"
#ifndef water_HPP
#define water_HPP
inline void waterinit(blockname::block* blk) {
	blk->mesh->setfaces(watertex,watertex,watertex,watertex,watertex,watertex);
	blk->solid = false;
	blk->transparent = true;
	blk->emitedlight = 0;
	blk->mesh->scale = blockname::unitscale;
}
inline void waterdelete (blockname::block* blk) {
	

}

#endif // !wood_HPP
