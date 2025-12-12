#pragma once
#include "block.h"
#include "../items/rope.h"
#include "../items/loottable.h"

inline void ropeinit(blockname::block* blk) {
	blk->mesh.setfaces(ropetex, ropetex, ropetex, ropetex, ropetex, ropetex);
	blk->mesh.box.scale = v3::Vec3(1 / 8.f, .5f, 1 / 8.f);
	blk->attributes.solid = true;
	blk->attributes.transparent = true;
	blk->emitedlight = 0;
	blk->minedfastwithpick = 1;
	blk->mininglevel = .3f;
	blk->createdefaultaabb(true);
	blk->owner().add_component<loottable>().addelem(ropeitem, 1, false);

	blk->minedfastwithpick = false;
}