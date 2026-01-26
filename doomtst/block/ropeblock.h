#pragma once
#include "block.h"

inline void ropeinit(blockname::block& blk) {
	blk.mesh.setfaces(ropetex, ropetex, ropetex, ropetex, ropetex, ropetex);
	blk.mesh.box.scale = v3::Scale3(1 / 8.f, .5f, 1 / 8.f);
	blk.attributes.solid = true;
	blk.attributes.transparent = true;
	blk.emitedlight = 0;
	blk.minedfastwithpick = 1;
	blk.mininglevel = .3f;
	blk.createdefaultaabb(true);
	blk.owner().add_component<loot_table>().add("rope", 1, false);

	blk.minedfastwithpick = false;
}