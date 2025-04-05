#include "block.h"
#ifndef torch_HPP
#define torch_HPP
inline void torchinit(blockname::block* blk) {
	blk->mesh.setfaces(torchtex,torchtex,torchtoptex,planktex,torchtex,torchtex);

	blk->attributes.solid = true;
	blk->attributes.transparent = true;
	blk->emitedlight =13;
	blk->lightval = 8;
	blk->mesh.box.scale = v3::Vector3(1 / 16.f, .375, 1 / 16.f);
	blk->mesh.attachindirection();

	blk->createdefaultaabb(false);
	blk->mininglevel = 1;
	blk->minedfastwithpick = true;
	blk->addcomponent<loottable>();
	blk->getcomponent<loottable>().addelem(torchitem, 1);
}

inline void torchdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !torch_HPP
