#include "geometry.h"
#include "Option.h"
#pragma once 
namespace geointersect {
	struct RayHit {
		float dist;
		v3::Vec3 intersectionpoint;
		ray Ray;
		RayHit() : dist(0.0f), intersectionpoint(v3::zerov) {}
		// Constructor with parameters
		RayHit(ray IntRay, float dist) : dist(dist) {
			intersectionpoint = IntRay.pointAt(dist);
			Ray = IntRay;
		}
	};
	using boxRayCollision = stn::Option<RayHit>;


	boxRayCollision intersection(geometry::Box box, ray fray);
	bool intersects(geometry::cone cone, geometry::Sphere sph);
}
 // !intersection_HPP
