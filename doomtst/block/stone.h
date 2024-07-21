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
	blk->addcomponent<loottable>();
		blk->getcomponent<loottable>().addelem(0, 1);
}
inline void stonedelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->getcomponent<loottable>().ondestroy();

}
#endif // !wood_HPP
