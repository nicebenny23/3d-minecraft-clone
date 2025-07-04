#include "block.h"
#include "liquid.h"
#include "../entities/onhit.h"
#include "../player/playerhealth.h"
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
	blk->owner.addcomponent<liquidprop>(5);

	blk->owner.addcomponent<dmgonhit<estate>>(1,0);
	blk->minedfastwithpick = false;
}
inline void lavadelete(blockname::block* blk) {
	blk->owner.removecomponent<liquidprop>();
	blk->owner.removecomponent < aabb::Collider>();
	blk->owner.removecomponent<dmgonhit<estate>>();
}

 // !wood_HPP
