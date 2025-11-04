#pragma once
#include "block.h"
#include "../items/loottable.h"
inline void plankinit(blockname::block* blk) {

	blk->mesh.setfaces(planktex, planktex, planktex, planktex, planktex, planktex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;
	blk->createdefaultaabb();
	blk->mininglevel = 1.5f;
	blk->owner.addcomponent<loottable>().addelem(plankitem, 1, false);
	blk->createdefaultaabb(false);
}