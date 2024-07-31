#include "block.h"
#ifndef obsidian_HPP
#define obsidian_HPP
inline void obsidianinit(blockname::block* blk) {

	blk->mesh.setfaces(obsidiantex, obsidiantex, obsidiantex, obsidiantex, obsidiantex, obsidiantex);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	blk->mininglevel = 5;
	blk->minedfastwithpick = true;
	//blk->addcomponentptr<loottable>()->addelem(obsidianitem, 2, true);
}
inline void obsidiandelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
#endif // !wood_HPP

