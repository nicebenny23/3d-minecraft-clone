#include "block.h"
#pragma once 
inline void torchinit(blocks::block& blk) {
	blk.mesh.set_face_textures(torchtex,torchtex,torchtoptex,planktex,torchtex,torchtex);

	blk.attributes.solid = true;
	blk.mesh.transparent = true;
	blk.owner().add_component<block_emmision>(13);
	blk.light_passing_through = 8;
	blk.mesh.box.scale = v3::Scale3(1 / 16.f, .375, 1 / 16.f);
	blk.mesh.attachindirection();

	blk.createdefaultaabb(false);
	blk.mininglevel = 1;
	blk.minedfastwithpick = true;
	blk.owner().add_component<loot_table>();
	blk.owner().get_component<loot_table>().add("moss", 1);
}
