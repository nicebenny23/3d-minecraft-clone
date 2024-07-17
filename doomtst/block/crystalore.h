#include "block.h"
#ifndef crystalore_HPP
#define crystalore_HPP
inline void crystalinit(blockname::block* blk) {

	blk->mesh.setfaces(crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex, crystaloretex);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
}
inline void crystaldelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
#endif // !wood_HPP

