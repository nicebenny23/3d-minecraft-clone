#include "grid.h"
#include "../util/ray.h"
#include "../game/aabb.h"
#include "../util/intersection.h"
#include "../util/geometry.h"
#include "../game/GameContext.h"
#ifndef voxtrav_Hpp
#define voxtrav_Hpp
namespace voxtra {

	struct RayWorldIntersection
	{
		aabb::Collider* collider;
		v3::Vector3 colpoint;
		ray Ray;
		float dist;
		bool intersected() {

			return (collider != nullptr);
		}
		RayWorldIntersection(float distance, aabb::Collider* closestbox, v3::Vector3 intpoint,ray emitedray) {
			Ray = emitedray;
			dist = distance;
			collider = closestbox;
			colpoint = intpoint;
		}
		RayWorldIntersection() {
			collider = nullptr;
			dist = -1;
			colpoint = zerov;
		}
	};
	enum GridTraverseMode {
		countnormal = 0,
		countall = 1,
		countsolid = 2,

	};


	bool Boxcollwithgrid(geometry::Box Box);
	RayWorldIntersection travvox(ray nray, float acc, GridTraverseMode trav = countnormal);
	block* findprevblock(ray nray, float acc, GridTraverseMode trav = countnormal);

	inline v3::Vector3 findemptyspace(v3::Vector3 scale) {
		geometry::Box loadbox;
		int test = 0;
		bool notinblock = false;
		do
		{


			float ranx = (random() - .5) * 2;

			float rany = (random() - .5) * 2;

			float ranz = (random() - .5) * 2;
			
			v3::Vector3 offset = (Vector3(ranx, rany, ranz) * (2 * CtxName::ctx.Grid->rad + 1) / 2 +CtxName::ctx.Grid->gridpos) * chunkaxis;
			test++;
			if (1000 < test)
			{
				return offset;
			}
			loadbox = geometry::Box(offset, scale);
		} while (Boxcollwithgrid(loadbox));
		return loadbox.center;
	}
	inline Vector3 findground(v3::Vector3 scale) {
		Vector3 pos = zerov;
		bool shouldcontinue = true;
		do
		{
			pos = findemptyspace(scale);
			geometry::Box testbox = geometry::Box(pos - Vector3(0, scale.y + .5, 0), scale);
			shouldcontinue = !Boxcollwithgrid(testbox);
			if (random()<.001)
			{
				shouldcontinue = false;
			}
		} while (shouldcontinue);
	
		return pos - Vector3(0, scale.y, 0);
	}
}
#endif