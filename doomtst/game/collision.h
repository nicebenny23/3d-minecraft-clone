#include "aabb.h"
#include "entity.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
using namespace aabb;
#ifndef collision_HPP
#define collision_HPP
constexpr auto collisioniterations = 3;
#define interactmaxrange 6
#define interactminrange 0
namespace collision {

	void update();
	

	bool boxCollidesWithEntity(geometry::Box blk, gameobject::obj* orgin = nullptr);

	bool aabbCollidesWithEntity(Collider* blk, gameobject::obj* orgin = nullptr);
	voxtra::RayWorldIntersection raycastall(ray nray, gameobject::obj* orgin=nullptr,voxtra::gridtrav travmode = voxtra::countnormal);

	voxtra::RayWorldIntersection raycastentity(ray nray, gameobject::obj* orgin=nullptr);
	void handleduelentitycollisions();
	void handleCollisionWithGrid(Collider& entity);
}
#endif