#include "block.h"
#ifndef crystalore_HPP
#define crystalore_HPP
inline void crystalinit(blockname::block* blk) {

	blk->mesh.setfaces(crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	blk->mininglevel = 4;
	blk->minedfastwithpick = true;
	blk->addcomponentptr<loottable>()->addelem(crystalitem, 2,true);
}
inline void crystaldelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !wood_HPP

