#pragma once
#include "block.h"
#include "../items/loottable.h"

inline void crystalinit(blockname::block& blk) {

	blk.mesh.setfaces(crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
	blk.mininglevel = 3;
	blk.minedfastwithpick = true;
	blk.owner().add_component<loottable>().addelem("crystal", 2, true);
}