#include "block.h"
#pragma once 
inline void torchinit(blockname::block* blk) {
	blk->mesh.setfaces(torchtex,torchtex,torchtoptex,planktex,torchtex,torchtex);

	blk->attributes.solid = true;
	blk->attributes.transparent = true;
	blk->emitedlight =13;
	blk->lightval = 8;
	blk->mesh.box.scale = v3::Vec3(1 / 16.f, .375, 1 / 16.f);
	blk->mesh.attachindirection();

	blk->createdefaultaabb(false);
	blk->mininglevel = 1;
	blk->minedfastwithpick = true;
	blk->owner.addcomponent<loottable>();
	blk->owner.getcomponent<loottable>().addelem(torchitem, 1);
}

inline void torchdelete(blockname::block* blk) {
	blk->owner.removecomponent<aabb::Collider>();
	blk->owner.removecomponent<loottable>();

}
 // !torch_HPP
