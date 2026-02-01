#pragma once
#include "block.h"
#include "../items/loottable.h"
inline void plankinit(blocks::block& blk) {

	blk.mesh.set_face_textures(planktex, planktex, planktex, planktex, planktex, planktex);
	blk.attributes.solid = true;
	blk.mesh.box.scale = blocks::blockscale;
	blk.createdefaultaabb();
	blk.mininglevel = 1.5f;
	blk.owner().add_component<loot_table>().add("plank", 1, false);
	blk.createdefaultaabb(false);
}