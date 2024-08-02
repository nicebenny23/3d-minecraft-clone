#include "block.h"
#include "../items/loottable.h"
#ifndef plank_HPP
#define plank_HPP
inline void plankinit(blockname::block* blk) {

	blk->mesh.setfaces(planktex, planktex, planktex, planktex, planktex, planktex);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();

	blk->addcomponentptr<loottable>()->addelem(plankitem, 1, false);
	blk->createaabb(false);
}
inline void plankdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
#endif // !wood_HPP
