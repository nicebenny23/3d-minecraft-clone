#pragma once
#include "block.h"
#include "../items/loottable.h"
#

inline void treestoneinit(blockname::block* blk) {

	blk->mesh.setfaces( treestonetex,treestonetex,logtoppng,logtoppng,treestonetex,treestonetex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	blk->owner->addcomponent<loottable>()->addelem(treestoneitem,1);
	blk->mininglevel = .9;
	blk->minedfastwithpick = false;
}
inline void treestonedelete(blockname::block* blk) {
	blk->owner->removecomponent<aabb::Collider>();

	blk->owner->removecomponent<loottable>();


}
 // !wood_HPP
