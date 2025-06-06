#include "block.h"
#include "../items/mossitem.h"
#include "../items/loottable.h"
#ifndef Moss_HPP
#define Moss_HPP
inline void mossinit(blockname::block* blk) {
	blk->mesh.setfaces(mosstex, mosstex, mosstex, mosstex, mosstex, mosstex);

	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->minedfastwithpick = 1;
	blk->mininglevel = .3f;
	blk->mesh.box.scale = blockname::blockscale;
	blk->createdefaultaabb();
	blk->addcomponent<loottable>()->addelem(mossitem, 1,true);

	blk->minedfastwithpick = false;
}
inline void mossdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !wood_HPP
