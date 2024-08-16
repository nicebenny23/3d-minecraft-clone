#include "grid.h"
#include "../util/ray.h"
#include "../game/aabb.h"
#include "../util/geometry.h"
#ifndef voxtrav_Hpp
#define voxtrav_Hpp
namespace voxtra {

	struct RayCollisionWithGrid
	{
		aabb::Collider* box;
		v3::Vector3 colpoint;
		float dist;

		RayCollisionWithGrid(float distance, aabb::Collider* closestbox, v3::Vector3 intpoint) {

			dist = distance;
			box = closestbox;
			colpoint = intpoint;
		}
		RayCollisionWithGrid() {
			box = nullptr;
			dist = 1;
			colpoint = zerov;
		}
	};
	enum gridtrav {
		countnormal = 0,
		countall = 1,
		countsolid = 2,

	};
	bool raycolllideswithgrid(ray nray, float acc, gridtrav trav =countnormal);
	bool Boxcollwithgrid(geometry::Box bx, gridtrav trav = countnormal);
	RayCollisionWithGrid travvox(ray nray, float acc, gridtrav trav = countnormal);
	block* findprevblock(ray nray, float acc, gridtrav trav = countnormal);
	inline v3::Vector3 findemptyspace(v3::Vector3 scale) {
		geometry::Box loadbox;
		int test = 0;
		bool notinblock = false;
		do
		{


			float ranx = (random() - .5) * 2;

			float rany = (random() - .5) * 2;

			float ranz = (random() - .5) * 2;
			
			v3::Vector3 offset = (Vector3(ranx, rany, ranz) * (2 * loadamt + 1) / 2 +grid::gridpos) * chunkaxis;
			test++;
			if (1000 < test)
			{
				return offset;
			}
			loadbox = geometry::Box(offset, scale);
		} while (Boxcollwithgrid(loadbox, countnormal));
		return loadbox.center;
	}
	inline Vector3 findground(v3::Vector3 scale) {
		Vector3 pos = zerov;
		bool shouldcontinue = true;
		do
		{
			pos = findemptyspace(scale);
			geometry::Box testbox = geometry::Box(pos - Vector3(0, scale.y + .5, 0), scale);
			shouldcontinue = !Boxcollwithgrid(testbox, countnormal);
			if (random()<.001)
			{
				shouldcontinue = false;
			}
		} while (shouldcontinue);
	
		return pos - Vector3(0, scale.y, 0);
	}
}
#endif