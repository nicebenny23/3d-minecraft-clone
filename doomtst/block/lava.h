#include "block.h"
#include "liquid.h"
#ifndef lava_HPP
#define lava_HPP
inline void lavainit(blockname::block* blk) {
	blk->mesh.setfaces(lavatex, lavatex, lavatex, lavatex, lavatex, lavatex);
	blk->solid = false;
	blk->transparent = true;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->addcomponent<liquidprop>(15);
	blk->createaabb(true);

	blk->minedfastwithpick = false;
}
inline void lavadelete(blockname::block* blk) {
	blk->removecomponent<liquidprop>();
	blk->removecomponent < aabb::Collider>();
}

#endif // !wood_HPP
