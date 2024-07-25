#include "block.h"
#include "liquid.h"
#ifndef water_HPP
#define water_HPP
inline void waterinit(blockname::block* blk) {
	blk->mesh.setfaces(watertex,watertex,watertex,watertex,watertex,watertex);
	blk->solid = false;
	blk->transparent = true;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->addcomponent<liquidprop>(15);
	blk->createaabb();

	blk->minedfastwithpick = false;
}
inline void waterdelete (blockname::block* blk) {
	blk->removecomponent<liquidprop>();
	blk->removecomponent < aabb::Collider>();
}

#endif // !wood_HPP
