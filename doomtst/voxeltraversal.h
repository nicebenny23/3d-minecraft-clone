#include "grid.h"
#include "ray.h"
#include "aabb.h"
#ifndef voxtrav_Hpp
#define voxtrav_HPP
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

#endif