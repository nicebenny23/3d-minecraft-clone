#include "geometry.h"
#include "../util/Option.h"

#include "interval.h"
#pragma once 
namespace geo {
	struct RayBox {
		
		v3::Scale3 scale;
		geo::ray ray;
		RayBox(geo::ray followed_ray, v3::Scale3 size) : ray(followed_ray), scale(size) {
		}
		Box bounding_box() const {
			return ray.bounding_box().expanded(scale);
		}
		Box end_box() const {
			return Box(ray.end, scale);
		}
		Box start_box() const {
			return Box(ray.start, scale);
		}
	};
	struct RayHit {
		geo::ray ray;
		double length() const{
			return ray.length();
		}

		geo::Box hit_box;
		stn::Option < math::Direction3d> hit_normal;
		RayHit(geo::ray ray, stn::Option<math::Direction3d> hit_normal, geo::Box hitbox) : ray(ray), hit_normal(hit_normal),hit_box(hitbox){
		}
	};
	using RayCollision = stn::Option<RayHit>;
	inline RayCollision intersection(geo::Box box, geo::ray ray) {
		v3::Vec3 dir = ray.dir();

		//not actually max
		math::bounds bounds = math::all_reals;
		int collision_axis = 0;
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
			if (intersection.unwrap().min()>bounds.min()) {
				collision_axis = i;
			}
			bounds = intersection.unwrap();
		}
		return 
			bounds.intersect(math::bounds(0, ray.length()))
			.map([&](math::bounds bounds) {
				if (bounds.min()==0) {
					return RayHit(ray.dialate_from_start(0), stn::None, box);
				}
				else {
						math::Direction3d intersection_direction=math::Directions3d[(math::sign_rounding_up(ray.dir()[collision_axis]) == 1) + 2 * (collision_axis)];
					return RayHit(ray.with_length(bounds.min()), intersection_direction,box);

				}
			});
	}
	
	//we use the minkoski sum 
	inline RayCollision intersection(RayBox ray_box, geo::Box box) {
		geo::Box expanded = box.expanded(ray_box.scale);
		return intersection(expanded, ray_box.ray);
	}
	inline bool intersects(geo::cone cone, geo::Sphere sph) {
		return cone.distance_from_point(sph.center) <= sph.radius;
	}
}
 // !intersection_HPP
