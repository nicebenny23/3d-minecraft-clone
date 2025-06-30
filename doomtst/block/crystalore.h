#pragma once
#include "block.h"
#include "../items/loottable.h"
inline void crystalinit(blockname::block* blk) {

	blk->mesh.setfaces(crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	blk->mininglevel = 4;
	blk->minedfastwithpick = true;
	blk->addcomponent<loottable>()->addelem(crystalitem, 2,true);
}
inline void crystaldelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
