#pragma once
#include "ray.h"
#include "vector3.h"
#include "mathutil.h"
#include "vector2.h"
using namespace v3;

namespace geometry {

	
	struct Box {
		Vec3 center;
		Vec3 scale;
		Box() {
			center = zerov;
			scale = zerov;
		}
		bool Boxinbox(Box b ) {
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
		bool contains(v3::Vec3 pos)
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
		bool contains_orgin()
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
		Box(Vec3 cent, Vec3 scl)
			: center(cent), scale(scl) {}

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


		float distanceToPoint(const Vec3& p) const {
			return dot(normal, p - point);
		}
	};

	struct sphere {
		float radius;
		Vec3 center;

		sphere(Box bx)
			: radius(mag(bx.scale)), center(bx.center) {}

		bool isonoraboveplane(const Plane& plane) {
			return plane.distanceToPoint(center) > -radius;
		}
	};

	struct cone {
		ray dirray;
		float slope;

		cone(const ray newray, float coneslope)
			: dirray(newray), slope(coneslope) {}

		

		float distanceFromPoint(Vec3 samplePoint) {
			// Project the sample point onto the cone's central axis (ray)
			Vec3 axisProjection = dirray.project(samplePoint);

			// Compute the perpendicular distance from the sample point to the cone's axis.
			float distanceToAxis = dist(samplePoint, axisProjection);

			// Determine how far along the axis (from the vertex) the projection lies.
			float distanceAlongAxis = dist(dirray.start, axisProjection);

			// At a distance 'distanceAlongAxis' along the axis, the cone's expected radius is:
			//     expectedRadius = slope * distanceAlongAxis.
			// The difference (distanceToAxis - expectedRadius) gives the radial offset from the cone's surface.
			// We divide by sqrt(1 + slope^2) to convert this radial difference into the true perpendicular distance
			// to the cone's surface (accounting for the cone's slanted shape).
			return (distanceToAxis - slope * distanceAlongAxis) / (sqrt(1 + slope * slope));
		}
	};

	inline bool  boxboxintersect(geometry::Box p1, geometry::Box p2)
	{
		return (p1 - p2).contains_orgin();
					
	}
inline 	v3::Vec3 collidebox(Box p1, Box p2)
	{
		int sgnx = p1.center.x > p2.center.x ? 1 : -1;
		int sgny = p1.center.y > p2.center.y ? 1 : -1;
		int sgnz = p1.center.z > p2.center.z ? 1 : -1;
		if (geometry::boxboxintersect(p1, p2)) {

			float xdepth = sgnx * (p1.scale.x + p2.scale.x) - (p1.center.x - p2.center.x);
			float ydepth = sgny * (p1.scale.y + p2.scale.y) - (p1.center.y - p2.center.y);
			float zdepth = sgnz * (p1.scale.z + p2.scale.z) - (p1.center.z - p2.center.z);
			if (abs(xdepth) < abs(ydepth) && abs(xdepth) < abs(zdepth)) {
				return v3::Vec3(xdepth, 0, 0);
			}
			if (abs(ydepth) < abs(zdepth)) {
				return v3::Vec3(0, ydepth, 0);
			}
			return v3::Vec3(0, 0, zdepth);
		}
		return v3::zerov;

	}
}

