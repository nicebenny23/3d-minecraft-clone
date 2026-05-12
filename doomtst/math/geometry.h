#pragma once
#include "ray.h"
#include "vector3.h"
#include "mathutil.h"
#include "vector2.h"
#include "../util/Option.h"
#include "dir.h"
#include "box.h"
using namespace v3;

namespace geo {
	


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
		v2::Vec2 transform_point(v2::Vec2 point) const{
			return center + point* scale;
		}
		v2::Vec2 transform_scale(v2::Vec2 other_scale) const {
			return other_scale* scale;
		}
		Box2d transform(Box2d other_transform) const {
			return Box2d(transform_point(other_transform.center), transform_scale(other_transform.scale));
		}
		bool contains(Box2d box) const {
			return contains(box.max()) && contains(box.min());
		}

		bool contains(v2::Vec2 point) const {

			point -= center;
			if (math::abs(point.x) <= scale.x/2 && math::abs(point.y) <= scale.y/2) {
				return true;
			}
			return false;
		}
		static Box2d origin_centered(v2::Vec2 scale) {
			return Box2d(v2::zerov, scale);
		}
		static Box2d from_min_max(v2::Vec2 min, v2::Vec2 max) {
			return Box2d((max + min) / 2, (max - min));
		}
	};
	inline constexpr Box2d unit_box_2d = Box2d(v2::Vec2(0, 0), v2::Vec2(1, 1));



	struct HalfSpace {
		Vec3 normal;
		Point3 point;

		HalfSpace() = default;
		HalfSpace(Vec3 positive_norm, Point3 pnt)
			: normal(positive_norm.normal()), point(pnt) {
		}
		HalfSpace(Point3 p1, Point3 p2, Point3 p3) {
			Vec3 v1 = p2 - p1;
			Vec3 v2 = p3 - p1;
			normal = v3::normal(Cross(v1, v2));
			point = p1;
		}
		bool contains(Point3 pnt) const{
			return 0 <= signed_distance_from_boundry_to_point(pnt);
		}
		
		//returns if it crosses a vector
		bool crosses(geo::ray potential_crossing) const {
			return contains(potential_crossing.start)^ contains(potential_crossing.end);
		}


		double signed_distance_from_boundry_to_point(const Point3& p) const {
			return v3::dot(normal, p - point);
		}
	};

	inline bool half_space_box_intersection(HalfSpace p1, geo::Box p2) {
		for (math::cube_index index : math::cube_indices) {
			if (p1.contains(p2.point_at_vertex(index))) {
				return true;
			};
		}
		return false;
	}
	struct Frustum {
		stn::List<HalfSpace,6> planes; 
		bool contains_point(v3::Point3 point) const {
			for (const HalfSpace& plane:planes) {
				if (!plane.contains(point)) {
					return false;
				}
			}
			return true;
		}

	};

	inline bool frustum_box_intersection(Frustum p1, geo::Box p2) {
		for (HalfSpace& half_space:p1.planes) {
			if (!half_space_box_intersection(half_space,p2)) {
				return false;
			}
		}
		
		return true;
	}
	struct Sphere {
		double radius;
		Point3 center;
		bool contains(Point3 point) const {
			return dist(center, point) <= radius;
		}
		Sphere(Point3 cent, double rad) :center(cent), radius(rad) {

		}
		Sphere(Box bx)
			: radius(Vec3(bx.half_size()).length()), center(bx.center) {
		}
		static Sphere bounding_sphere(v3::Point3 p1, v3::Point3 p2) {
			return Sphere(v3::midpoint(p1, p2), v3::dist(p1, p2) / 2);
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
		bool is_on_or_above_plane(const HalfSpace& plane) const {
			return plane.signed_distance_from_boundry_to_point(center) > -radius;
		}

	};

	struct cone {
		geo::ray direction;
		double slope;

		cone(const geo::ray newray, double cone_slope)
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
			Vec3 v = samplePoint - direction.start;

			double t = dot(v, direction.dir());

			if (t < 0) {
				return v.length();
			}
			Point3 axisProjection = direction.start + direction.dir() * t;
			double distanceToAxis = dist(samplePoint, axisProjection);
			return (distanceToAxis - slope * t) / sqrt(1 + slope * slope);
		}
	};

	inline bool boxes_intersect(geo::Box p1, geo::Box p2) {
		return (p1 - p2).contains_point(v3::Point3(0,0,0));

	}
	inline 	stn::Option<v3::Vec3> collide_box(Box p1, Box p2) {
		if (!geo::boxes_intersect(p1, p2)) {
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

