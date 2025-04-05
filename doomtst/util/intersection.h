#include "geometry.h"

#ifndef intersection_HPP
#define intersection_HPP
namespace geointersect {
	struct boxraycollision {
		bool collided;
		float dist;
		v3::Vector3 intersectionpoint;

		boxraycollision()
			: collided(false), dist(INFINITY), intersectionpoint(v3::zerov) {

		}

		boxraycollision(bool colided, float orgindist, v3::Vector3 pointofintersection)
			: collided(colided), dist(orgindist), intersectionpoint(pointofintersection) {}
	};


	  boxraycollision intersection(geometry::Box box, ray fray);
	bool intersects(geometry::cone cone, geometry::sphere sph);
}
#endif // !intersection_HPP
