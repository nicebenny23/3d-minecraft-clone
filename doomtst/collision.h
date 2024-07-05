#include "aabb.h"
#include "grid.h"
using namespace aabb;
#ifndef collision_HPP
#define collision_HPP
#define interactmaxrange 6
#define interactminrange 1
namespace collision {

	void update();
	

	struct raycolwithgrid
	{
		colrect* box;
		v3::Vector3 colpoint;
		float dist;
		raycolwithgrid(float dist1, colrect* closest1, v3::Vector3 intpoint) {

			dist = dist1;
			box = closest1;
			colpoint = intpoint;
		}
		raycolwithgrid() = default;
	};


	raycolwithgrid collideraywithgrid(ray nray);
	void	 collidecamray();
void collideobjwithgrid(colrect& entity);
}
#endif