#include "block.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#include "../world/voxeltraversal.h"
#include "../game/objecthelper.h"
#include "../items/recipe.h"
#pragma once 

inline void furnaceinit(blockname::block& blk) {
	
	blk.mesh.setfaces(stonetex, stonetex, furnacefront, stonetex, furnaceside, stonetex);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
	//stupid thing

}