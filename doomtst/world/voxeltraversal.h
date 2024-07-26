#include "grid.h"
#include "../util/ray.h"
#include "../game/aabb.h"
#include "../util/geometry.h"
#ifndef voxtrav_Hpp
#define voxtrav_Hpp
namespace voxtra {
	inline Coord   getcurrvoxel(Vector3 pos) {
		return  Coord(std::floor(pos.x), std::floor(pos.y), std::floor(pos.z));
	}
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

		bool notinblock = false;
		do
		{


			float ranx = (random() - .5) * 2;

			float rany = (random() - .5) * 2;

			float ranz = (random() - .5) * 2;
			v3::Vector3 offset = (Vector3(ranx, rany, ranz) * (2 * loadamt + 1) / 2 +grid::gridpos) * 16;
			loadbox = geometry::Box(offset, scale);
		} while (Boxcollwithgrid(loadbox, countnormal));
		return loadbox.center;
	}
}
#endif