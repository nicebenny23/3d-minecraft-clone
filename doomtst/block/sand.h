#include "block.h"
#include "../world/grid.h"
#include "../game/objecthelper.h"
#ifndef sand_HPP
#define sand_HPP/*
struct sandfall:gameobject::component{



	void update() {
		Coord pos = objutil::toblk(owner).pos;
		block* obj = grid::getobjatgrid(pos - Vector3(0, 1, 0), true);
		if ()
		{

		}
	}
}
inline void sandinit(blockname::block* blk) {

	blk->mesh.setfaces(0, 0, 0, 0, 0, 0);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	blk->createaabb(false);
}
inline void sanddelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
*/
#endif // !wood_HPP
