#pragma once
#include "block.h"
#include "../game/aabb.h"
#include "../items/loottable.h"
#include "../items/item.h"


inline void treestoneinit(blockname::block* blk) {

	blk->mesh.setfaces( treestonetex,treestonetex,logtoppng,logtoppng,treestonetex,treestonetex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	blk->owner.addcomponent<loottable>().addelem(treestoneitem,1);
	blk->mininglevel = 3;
	blk->minedfastwithpick = false;
}