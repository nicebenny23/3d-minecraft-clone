#include "block.h"
#ifndef grass_HPP
#define grass_HPP
inline void grassinit(blockname::block* blk) {
	blk->mesh->setfaces(dirttex, dirttex, grasstex, dirttex, dirttex, dirttex);

	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh->scale = blockname::unitscale;
	blk->createaabb();
}
inline void grassdelete(blockname::block* blk) {
	blk->removecomponent<aabb::colrect>();

}
#endif // !wood_HPP
