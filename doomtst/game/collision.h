#include "aabb.h"
#include "entity.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
using namespace aabb;
#ifndef collision_HPP
#define collision_HPP
#define interactmaxrange 6
#define interactminrange 0
namespace collision {

	void update();
	void sendplayercameraray();

	bool boxCollidesWithEntity(geometry::Box blk, gameobject::obj* orgin = nullptr);

	bool aabbCollidesWithEntity(Collider* blk, gameobject::obj* orgin = nullptr);
	voxtra::RayCollisionWithGrid raycastall(ray nray, gameobject::obj* orgin=nullptr,voxtra::gridtrav travmode = voxtra::countnormal);

	voxtra::RayCollisionWithGrid raycastentity(ray nray, gameobject::obj* orgin=nullptr);
	voxtra::RayCollisionWithGrid raycastCollisionWithGrid(ray nray);
	void handleduelentitycollisions();
	void handleCollisionWithGrid(Collider& entity);
}
#endif