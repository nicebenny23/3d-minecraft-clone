#include "block.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#pragma once 
inline void tableinit(blockname::block& blk) {

	blk.mesh.setfaces(craftingtableside, craftingtableside, craftingtabletop, craftingtableside, craftingtableside, craftingtableside);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
	//stupid thing
	//blk.owner().ensure_component<craftingtablecomp>();
//	blk.owner().add_component<loottable>().addelem(craftingtableitem, 1, false);
}