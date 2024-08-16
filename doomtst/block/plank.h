#include "block.h"
#include "../items/loottable.h"
#ifndef plank_HPP
#define plank_HPP
inline void plankinit(blockname::block* blk) {

	blk->mesh.setfaces(planktex, planktex, planktex, planktex, planktex, planktex);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	blk->mininglevel = .4;
	blk->addcomponent<loottable>()->addelem(plankitem, 1, false);
	blk->createaabb(false);
}
inline void plankdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
#endif // !wood_HPP
