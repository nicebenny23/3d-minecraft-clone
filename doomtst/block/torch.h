#include "block.h"
#ifndef torch_HPP
#define torch_HPP
inline void torchinit(blockname::block* blk) {
	blk->mesh.setfaces(torchtex,torchtex,torchtoptex,torchtoptex,torchtex,torchtex);

	blk->solid = true;
	blk->transparent = true;
	blk->emitedlight = 15;
	blk->lightval = 15;
	blk->mesh.scale = v3::Vector3(1 / 16.f, .375, 1 / 16.f);
	blk->mesh.attachindirection();
	blk->createaabb();

}

inline void torchdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

}
#endif // !torch_HPP