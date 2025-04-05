#include "block.h"
#ifndef ironore_HPP
#define ironore_HPP
inline void ironinit(blockname::block* blk) {

	blk->mesh.setfaces(ironoretex, ironoretex, ironoretex, ironoretex, ironoretex, ironoretex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	blk->mininglevel = 3;
	blk->minedfastwithpick = true;
	blk->addcomponent<loottable>()->addelem(ironoreitem, 1, false);
}
inline void irondelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !wood_HPP

