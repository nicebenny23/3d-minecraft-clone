#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "vector3.h"
#include "mathutil.h"
#include "ray.h"
#include "vector2.h"
using namespace v3;

namespace geometry {

	
	struct Box {
		Vector3 center;
		Vector3 scale;
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
		bool pointinbox(v3::Vector3 pos)
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

		Box(Vector3 cent, Vector3 scl)
			: center(cent), scale(scl) {}

		bool rayintersects(ray fray);
	};


	struct Box2d {
		v2::Vector2 center;
		v2::Vector2 scale;
		
		Box2d(v2::Vector2 cent, v2::Vector2 scl)
			: center(cent), scale(scl) {}

		bool pointinbox(v2::Vector2 point) {

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
		Vector3 normal;
		Vector3 point;

		Plane() = default;
		Plane(Vector3 norm, Vector3 pnt)
			: normal(norm), point(pnt) {}

		Plane(Vector3 p1, Vector3 p2, Vector3 p3) {
			Vector3 v1 = p2 - p1;
			Vector3 v2 = p3 - p1;
			normal = v3::normal(crossprod(v1, v2));
			point = p1;
		}


		float distanceToPoint(const Vector3& p) const {
			return dot(normal, p - point);
		}
	};

	struct sphere {
		float radius;
		Vector3 center;

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

		

		float distanceFromPoint(Vector3 samplePoint) {
			// Project the sample point onto the cone's central axis (ray)
			Vector3 axisProjection = dirray.projectpoint(samplePoint);

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
		if (abs(p1.center.x - p2.center.x) < p1.scale.x + p2.scale.x)
		{
			if (abs(p1.center.y - p2.center.y) < p1.scale.y + p2.scale.y)
			{
				if (abs(p1.center.z - p2.center.z) < p1.scale.z + p2.scale.z)
				{
					return true;
				}
			}
		}
		return false;
	}
inline 	v3::Vector3 collidebox(Box p1, Box p2)
	{
		int sgnx = p1.center.x > p2.center.x ? 1 : -1;
		int sgny = p1.center.y > p2.center.y ? 1 : -1;
		int sgnz = p1.center.z > p2.center.z ? 1 : -1;
		if (geometry::boxboxintersect(p1, p2)) {

			float xdepth = sgnx * (p1.scale.x + p2.scale.x) - (p1.center.x - p2.center.x);
			float ydepth = sgny * (p1.scale.y + p2.scale.y) - (p1.center.y - p2.center.y);
			float zdepth = sgnz * (p1.scale.z + p2.scale.z) - (p1.center.z - p2.center.z);
			if (abs(xdepth) < abs(ydepth)) {
				if (abs(xdepth) < abs(zdepth))
				{
					return v3::Vector3(xdepth, 0, 0);
				}
				return v3::Vector3(0, 0, zdepth);
			}
			if (abs(ydepth) < abs(zdepth))
			{
				return v3::Vector3(0, ydepth, 0);
			}
			return v3::Vector3(0, 0, zdepth);
		}
		return v3::zerov;

	}
}

#endif // GEOMETRY_HPP