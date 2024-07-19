#include "block.h"
#include "../items/loottable.h"
#ifndef dirt_HPP
#define dirt_HPP
inline void dirtinit(blockname::block* blk) {

	blk->mesh.setfaces( dirttex,dirttex,dirttex,dirttex,dirttex,dirttex);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	blk->addcomponent<loottable>();
	blk->getcomponent<loottable>().addelem(2, 1);
}
inline void dirtdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<loottable>();


}
#endif // !wood_HPP
