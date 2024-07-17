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
}
inline void stonedelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
#endif // !wood_HPP
