#pragma once
#include "block.h"
#include "../game/aabb.h"
#include "../items/loottable.h"
#include "../items/item.h"


inline void treestoneinit(blocks::block& blk) {

	blk.mesh.set_face_textures( treestonetex,treestonetex,logtoppng,logtoppng,treestonetex,treestonetex);
	blk.attributes.solid = true;
	blk.mesh.box.scale = blocks::blockscale;

	blk.createdefaultaabb(false);
	blk.owner().add_component<loot_table>().add("stone", 1);
	blk.mininglevel = 3;
	blk.minedfastwithpick = false;
}