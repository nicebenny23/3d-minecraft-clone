#include "grid.h"
#include "../util/ray.h"
#include "../game/aabb.h"
#ifndef voxtrav_Hpp
#define voxtrav_HPP
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
	raycolwithgrid() = default;
};

	raycolwithgrid travvox(ray nray,float acc);
	block* findprevblock(ray nray, float acc);

#endif