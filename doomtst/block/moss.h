#include "block.h"
#include "../items/loottable.h"
#pragma once 
inline void mossinit(blocks::block& blk) {
	blk.mesh.set_face_textures(mosstex, mosstex, mosstex, mosstex, mosstex, mosstex);

	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emited_light = 0;
	blk.minedfastwithpick = 1;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blocks::blockscale;
	blk.createdefaultaabb();
	blk.owner().add_component<loot_table>().add("moss", 1, true);

	blk.minedfastwithpick = false;
}