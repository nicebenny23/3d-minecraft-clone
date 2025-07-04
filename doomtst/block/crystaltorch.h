#pragma once
#include "block.h"
#include "../items/loottable.h"
inline void crystaltorchinit(blockname::block* blk) {
	blk->mesh.setfaces(crystaltorchtex,crystaltorchtex, crystaltorchtoptex, crystaltorchtoptex,crystaltorchtex, crystaltorchtex);

	blk->attributes.solid = true;
	blk->attributes.transparent = true;
	blk->emitedlight = 15;
	blk->lightval = 15;
	blk->mesh.box.scale = v3::Vector3(1 / 8.f, .375, 1 / 8.f);
	blk->mesh.attachindirection();

	blk->createdefaultaabb(false);
	blk->mininglevel = .25f;
	blk->minedfastwithpick = true;
	blk->owner.addcomponent<loottable>();
	blk->owner.getcomponent<loottable>().addelem(crystaltorchitem, 1);
}

inline void crystaltorchdelete(blockname::block* blk) {
	blk->owner.removecomponent<aabb::Collider>();
	blk->owner.removecomponent<loottable>();

}