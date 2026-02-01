#include "block.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../math/vector2.h"

#include "../items/recipe_file.h"
#include "../items/recipe_transactions.h"
#pragma once 
inline void tableinit(blocks::block& blk) {

	blk.mesh.set_face_textures(craftingtableside, craftingtableside, craftingtabletop, craftingtableside, craftingtableside, craftingtableside);
	blk.attributes.solid = true;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blocks::blockscale;

	blk.createdefaultaabb(false);
	//blk.owner().ensure_component<craftingtablecomp>();
//	blk.owner().add_component<loottable>().addelem(craftingtableitem, 1, false);
}