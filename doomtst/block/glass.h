#pragma once
#include "block.h"
inline void glassinit(blockname::block* blk) {

	blk->attributes.solid = true;
	blk->attributes.transparent = true;
	blk->mesh.setfaces(glasstex, glasstex, glasstex, glasstex, glasstex, glasstex);
	blk->emitedlight = 0;
	//blk->mesh.scale = blockname::unitscale;

	blk->minedfastwithpick = true;

	blk->createdefaultaabb(false);
}
inline void glassdelete(blockname::block* blk) {
	blk->owner.removecomponent<aabb::Collider>();

}