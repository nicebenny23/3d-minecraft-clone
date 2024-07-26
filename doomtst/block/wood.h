#include "block.h"
#ifndef wood_HPP
#define wood_HPP
inline void woodinit(blockname::block* blk) {

	blk->mesh.setfaces(0,0,0,0,0,0);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	blk->createaabb(false);
}
inline void wooddelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
#endif // !wood_HPP
