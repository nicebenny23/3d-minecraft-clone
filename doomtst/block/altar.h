#include "block.h"
#include "../items/loottable.h"
#ifndef altar_HPP
#define altar_HPP

inline void altarinit(blockname::block* blk) {
	blk->mesh.setfaces(altartex, altartex, obsidiantex, obsidiantex, altartex, altartex);

	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->minedfastwithpick = 1;
	blk->mininglevel = .3f;
	blk->mesh.box.scale = blockname::blockscale;
	blk->createdefaultaabb();
	blk->addcomponent<loottable>()->addelem(altaritem, 1, true);
	//CtxName::ctx.EntMan->shouldspawnfinalboss = true;
	blk->minedfastwithpick = false;
}
inline void altardelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !wood_HPP
