#include "grid.h"
#include "../math/ray.h"
#include "../game/aabb.h"
#include "../math/intersection.h"
#include "../math/geometry.h"
#include "../game/GameContext.h"
#include "../util/Option.h"
#include "../util/unique.h"
#pragma once 
namespace voxtra {

	struct RayWorldHit
	{
		aabb::Collider& collider;
		geointersect::RayHit Hit;
		RayWorldHit(geointersect::RayHit rayHit, aabb::Collider& WorldCollider) :Hit(rayHit),collider(WorldCollider){}
		Point3 intersection() const {
			return Hit.intersectionpoint;
		}
		
		ecs::obj owner() const {
			return collider.owner();
		}
		float dist() const {
			return Hit.dist;
		}
		ray ray() const {
			return Hit.Ray;
		}
	};
	using WorldRayCollision = stn::Option<RayWorldHit>;
	enum GridTraverseMode {
		countnormal = 0,
		countall = 1,
		countsolid = 2,

	};


	bool Boxcollwithgrid(geometry::Box Box);
	WorldRayCollision travvox(ray nray, GridTraverseMode trav = countnormal);
	block* findprevblock(ray nray, GridTraverseMode trav = countnormal);

	inline stn::Option<geometry::Box> findemptyspace(v3::Scale3 scale) {
		size_t max_tst = 40;
		for (size_t tst = 0; tst < max_tst; tst++)
		{
			double ranx = (random::random() - .5) * 2;
			double rany = (random::random() - .5) * 2;
			double ranz = (random::random() - .5) * 2;
			v3::Point3 test_pos = (Vec3(ranx, rany, ranz) * (2 * CtxName::ctx.Grid->rad + 1) / 2 + CtxName::ctx.Grid->gridpos) * chunkaxis;
			geometry::Box test_box = geometry::Box(test_pos,scale);
			if (!Boxcollwithgrid(test_box))
			{
				return stn::Option<geometry::Box>(test_box);
			}
		}

		return stn::None;
	}
	inline stn::Option<geometry::Box> findground(v3::Scale3 scale) {
		size_t max_checks = 100;
		for (size_t checks = 0; checks < max_checks; checks++)
		{


			stn::Option<geometry::Box> test_box = findemptyspace(scale);
			if (!test_box)
			{
				continue;
			}
			geometry::Box ground_tst_box = geometry::Box(test_box.unwrap().center - Vec3(0, scale.y, 0), v3::Scale3(scale.x, .1f, scale.z));;
		
			if (Boxcollwithgrid(ground_tst_box))
			{
				return test_box;
			}
		
		
		}
		return stn::None;
	}
}
