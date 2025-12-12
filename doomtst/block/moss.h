#include "block.h"
#include "../items/mossitem.h"
#include "../items/loottable.h"
#pragma once 
inline void mossinit(blockname::block* blk) {
	blk->mesh.setfaces(mosstex, mosstex, mosstex, mosstex, mosstex, mosstex);

	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->minedfastwithpick = 1;
	blk->mininglevel = 1;
	blk->mesh.box.scale = blockname::blockscale;
	blk->createdefaultaabb();
	blk->owner().add_component<loottable>().addelem(mossitem, 1,true);

	blk->minedfastwithpick = false;
}