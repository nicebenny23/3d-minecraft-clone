#include "aabb.h"
#include "grid.h"
using namespace aabb;
#ifndef collision_HPP
#define collision_HPP
namespace collision {

	void update();
	void collideraywithgrid();
void collideobjwithgrid(colrect& entity);
}
#endif