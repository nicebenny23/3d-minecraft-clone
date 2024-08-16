#include "block.h"
#include "liquid.h"
#include "../entities/onhit.h"
#ifndef lava_HPP
#define lava_HPP
inline void lavainit(blockname::block* blk) {
	blk->mesh.setfaces(lavatex, lavatex, lavatex, lavatex, lavatex, lavatex);
	blk->attributes.solid = false;
	blk->attributes.transparent = true;
	blk->emitedlight = 6;

	//`blk->lightval = 6;
	blk->mesh.scale = blockname::unitscale*.8;
	blk->createaabb(true);
	blk->mesh.scale = blockname::unitscale ;
	blk->addcomponent<liquidprop>(5);

	blk->addcomponent<dmgonhit>(1,"entity",0);
	blk->minedfastwithpick = false;
}
inline void lavadelete(blockname::block* blk) {
	blk->removecomponent<liquidprop>();
	blk->removecomponent < aabb::Collider>();
	blk->removecomponent<dmgonhit>();
}

#endif // !wood_HPP
