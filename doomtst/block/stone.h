#include "block.h"
#pragma once 
inline void stoneinit(blocks::block& blk) {
	blk.mesh.set_face_textures(stonetex, stonetex, stonetex, stonetex, stonetex, stonetex);

	blk.attributes.solid = true;
	blk.mesh.box.scale = blocks::blockscale;

	blk.createdefaultaabb(false);
	blk.mininglevel = 2;
	blk.minedfastwithpick = true;
	constexpr stn::HashedString hashed("moss");
	blk.owner().add_component<loot_table>().add(hashed, 1, false);
		
}