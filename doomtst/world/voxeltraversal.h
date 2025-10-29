#include "grid.h"
#include "../util/ray.h"
#include "../game/aabb.h"
#include "../util/intersection.h"
#include "../util/geometry.h"
#include "../game/GameContext.h"
#include "../util/Option.h"
#include "../util/UniquePtr.h"
#pragma once 
namespace voxtra {

	struct RayWorldHit
	{
		aabb::Collider* collider;

		geointersect::RayHit Hit;

		bool intersected() {

			return (collider != nullptr);
		}
		RayWorldHit(geointersect::RayHit rayHit, aabb::Collider* WorldCollider) {

			collider = WorldCollider;
			Hit = rayHit;
		}

		RayWorldHit() {
			collider = nullptr;

		}
		Vec3 intersection() {

			return Hit.intersectionpoint;
		}

		float dist() {
			return Hit.dist;
		}
		ray ray() {
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

	inline stn::Option<geometry::Box> findemptyspace(v3::Vec3 scale) {
		size_t max_tst = 40;
		for (size_t tst = 0; tst < max_tst; tst++)
		{
			float ranx = (random() - .5) * 2;
			float rany = (random() - .5) * 2;
			float ranz = (random() - .5) * 2;
			v3::Vec3 test_pos = (Vec3(ranx, rany, ranz) * (2 * CtxName::ctx.Grid->rad + 1) / 2 + CtxName::ctx.Grid->gridpos) * chunkaxis;
			geometry::Box test_box = geometry::Box(test_pos,scale);
			if (!Boxcollwithgrid(test_box))
			{
				return stn::Option<geometry::Box>(test_box);
			}
		}

		return stn::None;
	}
	inline stn::Option<geometry::Box> findground(v3::Vec3 scale) {
		size_t max_checks = 100;
		for (size_t checks = 0; checks < max_checks; checks++)
		{


			stn::Option<geometry::Box> test_box = findemptyspace(scale);
			if (!test_box)
			{
				continue;
			}
			geometry::Box ground_tst_box = geometry::Box(test_box().center - Vec3(0, scale.y, 0), v3::Vec3(scale.x, .1f, scale.z));;
		
			if (Boxcollwithgrid(ground_tst_box))
			{
				return test_box;
			}
		
		
		}
		return stn::None;
	}
}
