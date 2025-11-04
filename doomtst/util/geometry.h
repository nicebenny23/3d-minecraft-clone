#pragma once
#include "ray.h"
#include "vector3.h"
#include "mathutil.h"
#include "vector2.h"
#include "Option.h"
using namespace v3;

namespace geometry {

	
	struct Box {
		Vec3 center;
		//scale is not divided by 2
		Vec3 scale;
		Box() {
			center = zerov;
			scale = zerov;
		}
		Box(Vec3 cent, Vec3 scl) : center(cent), scale(scl) {}

		Vec3 max() const{
			return center + scale;
		}
		Vec3 min() const {
			return center-scale;
		}
		bool contains_orgin() const
		{
			if (abs(-center.x) <= scale.x)
			{
				if (abs(-center.y) <= scale.y)
				{
					if (abs(-center.z) <= scale.z)
					{
						return true;
					}
				}
			}
			return false;
		}

		bool contains(v3::Vec3 pos) const
		{
			pos -= center;
			if (abs(pos.x) <= scale.x)
			{
				if (abs(pos.y) <= scale.y)
				{
					if (abs(pos.z) <= scale.z)
					{
						return true;
					}
				}
			}
			return false;
		}

		bool contains_box(Box b) const{
			b.center -= center;
			if (abs(b.center.x)+b.scale.x<scale.x)
			{
				if (abs(b.center.y) + b.scale.y < scale.y)
				{
					if (abs(b.center.z) + b.scale.z < scale.z)
					{
						return true;
					}

				}

			}
			return false;
		}
		// Minkowski sum operator
		Box operator+(const Box& other) const {
			return Box(center + other.center, scale + other.scale);
		}
		// Minkowski difference operator
		Box operator-(const Box& other) const {
			return Box(center - other.center, scale + other.scale);
		}
		Box translate(Vec3 translation_vector) const{
			return Box(center + translation_vector, scale);
		}
		Box scale_from_center(float dialation) const{
			return Box(center , scale* dialation);
		}
		bool rayintersects(ray fray);
	};


	struct Box2d {
		v2::Vec2 center;
		v2::Vec2 scale;
		
		Box2d(v2::Vec2 cent, v2::Vec2 scl)
			: center(cent), scale(scl) {}

		bool contains(v2::Vec2 point) {

			point -= center;
			if (abs(point.x)<scale.x&&abs(point.y)<scale.y)
			{
				return true;
			}
			return false;    
		}

		Box2d() = default;
	};



	struct Plane {
		Vec3 normal;
		Vec3 point;

		Plane() = default;
		Plane(Vec3 norm, Vec3 pnt)
			: normal(norm), point(pnt) {}

		Plane(Vec3 p1, Vec3 p2, Vec3 p3) {
			Vec3 v1 = p2 - p1;
			Vec3 v2 = p3 - p1;
			normal = v3::normal(Cross(v1, v2));
			point = p1;
		}
		bool above(Vec3 pnt) const{
			return dot(point,normal)<= dot(normal,pnt);
		}
		bool crosses(ray potential_crossing) const{
			return above(potential_crossing.start) != above(potential_crossing.end);
		}


		double distance_to_point(const Vec3& p) const {
			return dot(normal, p - point);
		}
	};

	struct Sphere {
		double radius;
		Vec3 center;
		bool contains(Vec3 point) const{
			return dist(center, point) <= radius;
		}
		
		Sphere(Box bx)
			: radius(bx.scale.length()), center(bx.center) {}
		
		Vec3 project(Vec3 point) const{
			return center + normal(point - center) * radius;
		}
		Vec3 bound(Vec3 point) const {
			if (contains(point))
			{
				return point;
			}
			return project(point);
		}
		bool is_on_or_above_plane(const Plane& plane) const{
			return plane.distance_to_point(center) > -radius;
		}
		
	};

	struct cone {
		ray direction_ray;
		double slope;

		cone(const ray newray, double cone_slope)
			: direction_ray(newray), slope(cone_slope) {}

		//returns the normized direction
		Vec3 direction() const{
			return direction_ray.dir();
		}
		Vec3 orgin() const{
			return direction_ray.start;
		}

		double distanceFromPoint(Vec3 samplePoint) const{
			// Project the sample point onto the cone's central axis (ray)
			Vec3 axisProjection = direction_ray.project(samplePoint);

			// Compute the perpendicular distance from the sample point to the cone's axis.
			double distanceToAxis = dist(samplePoint, axisProjection);

			// Determine how far along the axis (from the vertex) the projection lies.
			double distanceAlongAxis = dist(direction_ray.start, axisProjection);

			// At a distance 'distanceAlongAxis' along the axis, the cone's expected radius is:
			//     expectedRadius = slope * distanceAlongAxis.
			// The difference (distanceToAxis - expectedRadius) gives the radial offset from the cone's surface.
			// We divide by sqrt(1 + slope^2) to convert this radial difference into the true perpendicular distance
			// to the cone's surface (accounting for the cone's slanted shape).
			return (distanceToAxis - slope * distanceAlongAxis) / (sqrt(1 + slope * slope));
		}
	};

	inline bool  boxes_intersect(geometry::Box p1, geometry::Box p2)
	{
		return (p1 - p2).contains_orgin();
					
	}
	inline 	stn::Option<v3::Vec3> collide_box(Box p1, Box p2)
	{
		if (!geometry::boxes_intersect(p1, p2)) {
			return stn::None;

		}
		Vec3 min_vector = zerov;
		double min_depth=std::numeric_limits<double>().infinity();
		for (int i = 0; i < 3; i++)
		{
			int sgn = p1.center[i] > p2.center[i] ? 1 : -1;
			double depth = sgn * (p1.scale[i] + p2.scale[i]) - (p1.center[i] - p2.center[i]);
			if (abs(depth)<=min_depth )
			{
				min_depth = abs(depth);
				min_vector = Vec3(depth, i);
			}
		}
		return min_vector;
		
	}
}

