#include "block.h"
#include "../items/loottable.h"
#ifndef crystaltorch_HPP
#define crystaltorch_HPP
inline void crystaltorchinit(blockname::block* blk) {
	blk->mesh.setfaces(crystaltorchtex,crystaltorchtex, crystaltorchtoptex, crystaltorchtoptex,crystaltorchtex, crystaltorchtex);

	blk->attributes.solid = true;
	blk->attributes.transparent = true;
	blk->emitedlight = 15;
	blk->lightval = 15;
	blk->mesh.scale = v3::Vector3(1 / 8.f, .375, 1 / 8.f);
	blk->mesh.attachindirection();

	blk->createaabb(false);
	blk->mininglevel = .25f;
	blk->minedfastwithpick = true;
	blk->addcomponent<loottable>();
	blk->getcomponent<loottable>().addelem(crystaltorchitem, 1);
}

inline void crystaltorchdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();
	blk->removecomponent<loottable>();

}
#endif // !torch_HPP
