#include "block.h"
#include "../items/loottable.h"
#
#ifndef treestone_HPP
#define treestone_HPP
inline void treestoneinit(blockname::block* blk) {

	blk->mesh.setfaces( treestonetex,treestonetex,logtoppng,logtoppng,treestonetex,treestonetex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	blk->addcomponent<loottable>();
	blk->mininglevel = .9;
	blk->minedfastwithpick = false;
	blk->getcomponent<loottable>().addelem(treestoneitem, 1);
}
inline void treestonedelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<loottable>();


}
#endif // !wood_HPP
