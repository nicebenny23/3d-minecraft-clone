#include "block.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#include "../items/recipe.h"
#pragma once 
inline void chestinit(blockname::block& blk) {

	blk.mesh.setfaces(chestside, chestside, chestside, chestside, chestfront, chestside);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
}
