#include "block.h"
#ifndef ironore_HPP
#define ironore_HPP
inline void ironinit(blockname::block* blk) {

	blk->mesh.setfaces(ironoretex, ironoretex, ironoretex, ironoretex, ironoretex, ironoretex);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	blk->mininglevel = 3;
	blk->minedfastwithpick = true;
	blk->addcomponentptr<loottable>()->addelem(crystalitem, 2, true);
}
inline void irondelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !wood_HPP

