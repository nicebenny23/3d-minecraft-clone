#include "aabb.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
using namespace aabb;
#ifndef collision_HPP
#define collision_HPP
#define interactmaxrange 6
#define interactminrange 0
namespace collision {

	void update();
	



	bool aabbCollidesWithEntity(Collider* blk);
	voxtra::RayCollisionWithGrid raycastCollisionWithGrid(ray nray);
	void handleduelentitycollisions();
void handleCollisionWithGrid(Collider& entity);
}
#endif