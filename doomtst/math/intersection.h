#include "geometry.h"
#include "../util/Option.h"

#include "interval.h"
#pragma once 
namespace geointersect {
	using RayHit = math::ray;
	struct RayBox {
		math::ray ray;
		math::Box box;
	};
	using boxRayCollision = stn::Option<RayHit>;
	inline stn::Option<math::bounds> intersection_interval(math::Box box, math::ray ray) {

		v3::Vec3 dir = ray.dir();

		//not actually max
		math::bounds bounds = math::all_reals;
		for (int i = 0; i < 3; i++) {
			double tst_val_1 = box.center[i] - box.half_size()[i] - ray.start[i];
			double tst_val_2 = box.center[i] + box.half_size()[i] - ray.start[i];
			if (math::approximate_equals(dir[i], 0)) {
				if (!math::bounds(tst_val_1, tst_val_2).contains(0)) {
					return stn::None;
				}
				continue;

			}
			math::bounds axis_bounds = math::bounds(tst_val_1, tst_val_2) / dir[i];
			stn::Option<math::bounds> intersection = bounds.intersect(axis_bounds);
			if (!intersection) {
				return stn::None;
			}
			bounds = intersection.unwrap();
		}
		return bounds;
	}

	inline boxRayCollision intersection(math::Box box, math::ray ray) {
		return 
		intersection_interval(box,ray)
		.and_then([&](math::bounds bounds) {return bounds.intersect(math::bounds(0, ray.length())); })
		.map([&](math::bounds bounds) {return ray.with_length(bounds.min()); });
	}
	inline bool intersects(math::cone cone, math::Sphere sph) {
		return cone.distance_from_point(sph.center) <= sph.radius;
	}
}
 // !intersection_HPP
