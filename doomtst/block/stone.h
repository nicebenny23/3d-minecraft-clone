#include "block.h"
#ifndef stone_HPP
#define stone_HPP
inline void stoneinit(blockname::block* blk) {
	blk->mesh.setfaces(stonetex, stonetex, stonetex, stonetex, stonetex, stonetex);

	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	blk->mininglevel = 1;
	blk->minedfastwithpick = true;
	blk->addcomponentptr<loottable>()->addelem(stoneitem,1,false);
		
}
inline void stonedelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<loottable>();


}
#endif // !wood_HPP
