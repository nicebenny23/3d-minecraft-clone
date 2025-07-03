#include "block.h"
#pragma once 
inline void stoneinit(blockname::block* blk) {
	blk->mesh.setfaces(stonetex, stonetex, stonetex, stonetex, stonetex, stonetex);

	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	blk->mininglevel = 2;
	blk->minedfastwithpick = true;
	blk->owner->addcomponent<loottable>()->addelem(stoneitem,1,false);
		
}
inline void stonedelete(blockname::block* blk) {
	blk->owner->removecomponent<aabb::Collider>();

	blk->owner->removecomponent<loottable>();


}
 // !wood_HPP
