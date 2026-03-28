#pragma once
#include "ray.h"
#include "vector3.h"
#include "mathutil.h"
#include "vector2.h"
#include "../util/Option.h"
#include "dir.h"
using namespace v3;

namespace math {
	struct Box {
		Point3 center;
		Scale3 scale;

		Box(Point3 cent, Scale3 scl) : center(cent), scale(scl) {
			int l = 3;
		}

		Point3 max() const {
			return center + Vec3(half_size());
		}
		Point3 min() const {
			return center - Vec3(half_size());
		}
		
		bool contains_point(Point3 pos) const {
			Vec3 shifted = center - pos;
			Scale3 bounds = half_size();
			if (abs(shifted.x) <= bounds.x) {
				if (abs(shifted.y) <= bounds.y) {
					if (abs(shifted.z) <= bounds.z) {
						return true;
					}
				}
			}
			return false;
		}
		v3::Point3 in_direction(math::Direction3d direction) {
			return direction.vec()* half_size() + center;
		}

		bool contains_box(Box b) const {
			return contains_point(b.max()) && contains_point(b.min());
		}
		Scale3 half_size() const {
			return scale / 2;
		}
		// Minkowski difference operator
		Box operator-(const Box& other) const {
			//minkoski diffrence changes affinity
			return Box(center .offset_local(other.center*-1), scale.expanded(other.scale));
		}
		Box translate(Vec3 translation_vector) const {
			return Box(center + translation_vector, scale);
		}
		Box scale_from_center(float dialation) const {
			return Box(center, scale * dialation);
		}
		
		Box transform(Box other_transform) const {
			return Box(center.offset_local(other_transform.center * half_size()), other_transform.scale * scale);
		}
	};


	struct Box2d {
		v2::Vec2 center;
		v2::Vec2 scale;
		double x_scale() const {
			return scale.x;
		}
		double y_scale() const {
			return scale.y;
		}
		constexpr Box2d(v2::Vec2 cent, v2::Vec2 scl)
			: center(cent), scale(scl) {
		}
		v2::Vec2 max() const {
			return center + scale / 2;
		}
		v2::Vec2 min() const {
			return center - scale / 2;
		}
		v2::Vec2 half_size() const {
			return scale / 2;
		}

		Box2d transform(Box2d other_transform) const {
			return Box2d(center + other_transform.center * scale, other_transform.scale * scale);
		}
		bool contains(Box2d box) const {
			return contains(box.max()) && contains(box.min());
		}
		bool contains(v2::Vec2 point) const {

			point -= center;
			if (abs(point.x) <= scale.x && abs(point.y) <= scale.y) {
				return true;
			}
			return false;
		}
		static Box2d origin_centered(v2::Vec2 scale) {
			return Box2d(v2::zerov, scale);
		}
		static Box2d from_min_max(v2::Vec2 min, v2::Vec2 max) {
			return Box2d((max + min) / 2, (max - min) / 2);
		}
	};
	inline constexpr Box2d unit_box_2d = Box2d(v2::Vec2(0, 0), v2::Vec2(1, 1));



	struct Plane {
		Vec3 normal;
		Point3 point;

		Plane() = default;
		Plane(Vec3 norm, Point3 pnt)
			: normal(norm), point(pnt) {
		}

		Plane(Point3 p1, Point3 p2, Point3 p3) {
			Vec3 v1 = p2 - p1;
			Vec3 v2 = p3 - p1;
			normal = v3::normal(Cross(v1, v2));
			point = p1;
		}
		bool above(Point3 pnt) const {
			return 0 <= dot(normal, pnt - point);
		}
		//returns if it crosses a vector
		bool crosses(math::ray potential_crossing) const {
			return above(potential_crossing.start) != above(potential_crossing.end);
		}


		double distance_to_point(const Point3& p) const {
			return dot(normal, p - point);
		}
	};

	struct Sphere {
		double radius;
		Point3 center;
		bool contains(Point3 point) const {
			return dist(center, point) <= radius;
		}

		Sphere(Box bx)
			: radius(Vec3(bx.half_size()).length()), center(bx.center) {
		}

		//returns the closest point on the spheres surface to the point 
		Point3 project(Point3 point) const {
			return center + normal(point - center) * radius;
		}
		//returns the closest point inside the sphere to the point 
		Point3 bound(Point3 point) const {
			if (contains(point)) {
				return point;
			}
			return project(point);
		}
		bool is_on_or_above_plane(const Plane& plane) const {
			return plane.distance_to_point(center) > -radius;
		}

	};

	struct cone {
		math::ray direction;
		double slope;

		cone(const math::ray newray, double cone_slope)
			: direction(newray), slope(cone_slope) {
		}

		//returns the normalized direction
		Vec3 normal_direction() const {
			return direction.dir();
		}
		Point3 orgin() const {
			return direction.start;
		}

		double distance_from_point(Point3 samplePoint) const {
			// Project the sample point onto the cone's central axis (ray)
			Point3 axisProjection = direction.project(samplePoint);

			// Compute the perpendicular distance from the sample point to the cone's axis.
			double distanceToAxis = dist(samplePoint, axisProjection);

			// Determine how far along the axis (from the vertex) the projection lies.
			double distanceAlongAxis = dist(direction.start, axisProjection);

			// At a distance 'distanceAlongAxis' along the axis, the cone's expected radius is:
			//     expectedRadius = slope * distanceAlongAxis.
			// The difference (distanceToAxis - expectedRadius) gives the radial offset from the cone's surface.
			// We divide by sqrt(1 + slope^2) to convert this radial difference into the true perpendicular distance
			// to the cone's surface (accounting for the cone's slanted shape).
			return (distanceToAxis - slope * distanceAlongAxis) / (sqrt(1 + slope * slope));
		}
	};

	inline bool boxes_intersect(math::Box p1, math::Box p2) {
		return (p1 - p2).contains_point(v3::Point3(0,0,0));

	}
	inline 	stn::Option<v3::Vec3> collide_box(Box p1, Box p2) {
		if (!math::boxes_intersect(p1, p2)) {
			return stn::None;

		}
		Vec3 min_vector = zerov;

		double min_depth = std::numeric_limits<double>().infinity();
		for (int i = 0; i < 3; i++) {
			int sgn = p1.center[i] > p2.center[i] ? 1 : -1;
			double depth = sgn * (p1.half_size()[i] + p2.half_size()[i]) - (p1.center[i] - p2.center[i]);
			if (abs(depth) <= min_depth) {
				min_depth = abs(depth);
				min_vector = Vec3(depth, i);
			}
		}
		return min_vector;

	}
}

