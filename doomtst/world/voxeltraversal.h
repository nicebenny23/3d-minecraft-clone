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
	struct raycolwithgrid
	{
		aabb::colrect* box;
		v3::Vector3 colpoint;
		float dist;

		raycolwithgrid(float distance, aabb::colrect* closestbox, v3::Vector3 intpoint) {

			dist = distance;
			box = closestbox;
			colpoint = intpoint;
		}
		raycolwithgrid() {
			box = nullptr;
			dist = 1;
			colpoint = zerov;
		}
	};
	bool Boxcollwithgrid(geometry::Box bx, float acc);
	raycolwithgrid travvox(ray nray, float acc);
	block* findprevblock(ray nray, float acc);
}
#endif