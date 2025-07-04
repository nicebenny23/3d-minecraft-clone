#include "block.h"
#pragma once 
inline void obsidianinit(blockname::block* blk) {

	blk->mesh.setfaces(obsidiantex, obsidiantex, obsidiantex, obsidiantex, obsidiantex, obsidiantex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	blk->mininglevel = 5;
	blk->minedfastwithpick = true;
	blk->owner.addcomponent<loottable>()->addelem(obsidianitem, 1, false);
}
inline void obsidiandelete(blockname::block* blk) {
	blk->owner.removecomponent<aabb::Collider>();
	blk->owner.removecomponent<loottable>();
}
 // !wood_HPP

