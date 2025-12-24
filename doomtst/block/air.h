#include "block.h"
#pragma once 
//makes a block air
inline void setdefault(blockname::block& blk) {
	blk.attributes.solid = false;
	blk.attributes.transparent = true;
	blk.emitedlight = 0;
	blk.id = minecraftair;
	blk.mesh.setfaces(0, 0, 0, 0, 0, 0);
	blk.mesh.box.center = blk.center();


	blk.minedfastwithpick = false;
	blk.mininglevel = 0;
	blk.lightval = 0;
}


// !wood_HPP
