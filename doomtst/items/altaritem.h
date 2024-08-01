#include "item.h"
#include "../items/loottable.h"
#ifndef altaritem_HPP
#define altaritem_HPP
#define altaritemblockid_HPP
inline void altariteminit(blockname::block* blk) {
	blk->mesh.setfaces(mosstex, mosstex, mosstex, mosstex, mosstex, mosstex);

	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->minedfastwithpick = 1;
	blk->mininglevel = .3f;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	blk->addcomponentptr<loottable>()->addelem(mossitem, 1, true);

	blk->minedfastwithpick = false;
}
inline void altardelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !wood_HPP
