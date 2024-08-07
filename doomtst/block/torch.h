#include "block.h"
#ifndef torch_HPP
#define torch_HPP
inline void torchinit(blockname::block* blk) {
	blk->mesh.setfaces(torchtex,torchtex,torchtoptex,torchtoptex,torchtex,torchtex);

	blk->solid = true;
	blk->transparent = true;
	blk->emitedlight = 8;
	blk->lightval = 8;
	blk->mesh.scale = v3::Vector3(1 / 16.f, .375, 1 / 16.f);
	blk->mesh.attachindirection();

	blk->createaabb(false);
	blk->mininglevel = .25f;
	blk->minedfastwithpick = true;
	blk->addcomponent<loottable>();
	blk->getcomponent<loottable>().addelem(torchitem, 1);
}

inline void torchdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !torch_HPP
