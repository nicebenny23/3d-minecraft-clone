#include "block.h"
#include "block_registry.h"
#pragma once 
//makes a block air
inline void setdefault(blocks::block& blk) {
	blk.attributes.solid = false;
	blk.block_id = minecraftair;
	blk.mesh.set_face_textures(treestonetex, treestonetex, treestonetex, treestonetex, treestonetex, treestonetex);
	blk.mesh.box.center = blk.center();


	blk.minedfastwithpick = false;
	blk.mininglevel = 0;
	blk.light_passing_through = 0;
}
struct air_block :BlockType {


};

// !wood_HPP
