#include "block.h"
#ifndef obsidian_HPP
#define obsidian_HPP
inline void obsidianinit(blockname::block* blk) {

	blk->mesh.setfaces(obsidiantex, obsidiantex, obsidiantex, obsidiantex, obsidiantex, obsidiantex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	blk->mininglevel = 5;
	blk->minedfastwithpick = true;
	blk->addcomponent<loottable>()->addelem(obsidianitem, 1, false);
}
inline void obsidiandelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();
}
#endif // !wood_HPP

