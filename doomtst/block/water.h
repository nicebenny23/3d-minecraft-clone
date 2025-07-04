#include "block.h"
#include "liquid.h"
#ifndef water_HPP
#define water_HPP
inline void waterinit(blockname::block* blk) {
	blk->mesh.setfaces(watertex,watertex,watertex,watertex,watertex,watertex);
	blk->attributes.solid = false;
	blk->attributes.transparent = true;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;
	blk->owner.addcomponent<liquidprop>(15);
	blk->createdefaultaabb(true);

	blk->minedfastwithpick = false;
}
inline void waterdelete (blockname::block* blk) {
	blk->owner.removecomponent<liquidprop>();
	blk->owner.removecomponent < aabb::Collider>();
}

#endif // !wood_HPP
