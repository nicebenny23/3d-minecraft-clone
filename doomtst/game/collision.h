#include "aabb.h"
#include "entity.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
#include "../util/Option.h"
using namespace aabb;
#pragma once 
constexpr auto collisioniterations = 6;
#define interactmaxrange 6
#define interactminrange 0
namespace collision {

	void update();
	struct HitQuery
	{
		Opt::Option<gameobject::obj> orgin;
		explicit HitQuery():orgin(Opt::None) {
		}
		explicit HitQuery(const gameobject::obj& orgin_obj): orgin(orgin_obj){

		}

	};
	
	bool boxCollidesWithEntity(geometry::Box blk, HitQuery query);

	bool aabbCollidesWithEntity(Collider* blk, HitQuery query);
	voxtra::WorldRayCollision raycastall(ray nray, HitQuery query, voxtra::GridTraverseMode travmode = voxtra::countnormal);

	voxtra::WorldRayCollision raycastentity(ray nray,HitQuery query);
	void handleduelentitycollisions();
	void handleCollisionWithGrid(Collider& entity,bool is_trigger);
}
