#include "block.h"
#pragma once 
inline void ironinit(blockname::block& blk) {

	blk.mesh.setfaces(ironoretex, ironoretex, ironoretex, ironoretex, ironoretex, ironoretex);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
	blk.mininglevel = 3;
	blk.minedfastwithpick = true;
	blk.owner().add_component<loottable>().addelem("iron", 1, false);
}