#include "block.h"
#include "liquid.h"
#include "../entities/onhit.h"
#pragma once 
inline void lavainit(blockname::block* blk) {
	blk->mesh.setfaces(lavatex, lavatex, lavatex, lavatex, lavatex, lavatex);
	blk->attributes.solid = false;
	blk->attributes.transparent = true;
	blk->emitedlight = 6;

	//`blk->lightval = 6;
	blk->mesh.box.scale = blockname::blockscale*.8;
	blk->createdefaultaabb(true);
	blk->mesh.box.scale = blockname::blockscale ;
	blk->addcomponent<liquidprop>(5);

	blk->addcomponent<dmgonhit>(1,"entity",0);
	blk->minedfastwithpick = false;
}
inline void lavadelete(blockname::block* blk) {
	blk->removecomponent<liquidprop>();
	blk->removecomponent < aabb::Collider>();
	blk->removecomponent<dmgonhit>();
}

 // !wood_HPP
