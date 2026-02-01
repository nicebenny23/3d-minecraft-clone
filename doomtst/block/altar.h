#include "block.h"
#include "../items/loottable.h"
#pragma once 
inline void altarinit(blocks::block& blk) {
	blk.mesh.set_face_textures(altartex, altartex, obsidiantex, obsidiantex, altartex, altartex);

	blk.attributes.solid = true;
	blk.minedfastwithpick = 1;
	blk.mininglevel = .3f;
	blk.mesh.box.scale = blocks::blockscale;
	blk.createdefaultaabb();
	blk.owner().add_component<loot_table>().add("altar item", 1, true);
	//CtxName::ctx.EntMan->shouldspawnfinalboss = true;
	blk.minedfastwithpick = false;
}