#include "aabb.h"
#include "entity.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
using namespace aabb;
#pragma once 
constexpr auto collisioniterations = 3;
#define interactmaxrange 6
#define interactminrange 0
namespace collision {

	void update();
	

	bool boxCollidesWithEntity(geometry::Box blk, gameobject::obj* orgin = nullptr);

	bool aabbCollidesWithEntity(Collider* blk, gameobject::obj* orgin = nullptr);
	voxtra::WorldRayCollision raycastall(ray nray, gameobject::obj* orgin=nullptr,voxtra::GridTraverseMode travmode = voxtra::countnormal);

	voxtra::WorldRayCollision raycastentity(ray nray, gameobject::obj* orgin=nullptr);
	void handleduelentitycollisions();
	void handleCollisionWithGrid(Collider& entity);
}
