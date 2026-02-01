#include "block.h"
#include "liquid.h"
#ifndef water_HPP
#define water_HPP
inline void waterinit(blocks::block& blk) {
	blk.mesh.set_face_textures(watertex,watertex,watertex,watertex,watertex,watertex);
	blk.attributes.solid = false;
	blk.mesh.transparent = true;
	blk.mesh.box.scale = blocks::blockscale;
	blk.owner().add_component<liquidprop>(15);
	blk.createdefaultaabb(true);

	blk.minedfastwithpick = false;
}
#endif