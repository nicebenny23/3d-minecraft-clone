#include "block.h"
#include "liquid.h"
#include "../entities/dmgonhit.h"
#ifndef lava_HPP
#define lava_HPP
inline void lavainit(blockname::block* blk) {
	blk->mesh.setfaces(lavatex, lavatex, lavatex, lavatex, lavatex, lavatex);
	blk->solid = false;
	blk->transparent = true;
	blk->emitedlight = 6;
	blk->mesh.scale = blockname::unitscale;
	blk->addcomponent<liquidprop>(5);
	blk->createaabb(true);
	blk->addcomponent<dmgplayeronhit>(1,"entity",false);
	blk->minedfastwithpick = false;
}
inline void lavadelete(blockname::block* blk) {
	blk->removecomponent<liquidprop>();
	blk->removecomponent < aabb::Collider>();
	blk->removecomponent<dmgplayeronhit>();
}

#endif // !wood_HPP
