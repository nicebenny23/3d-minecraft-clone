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
			if (abs(point.x)<scale.x,abs(point.y)<scale.y)
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

		float distancetoorgin() {
			// Function body needed
		}

		float distanceToPoint(const Vector3& p) const {
			return dotproduct(normal, p - point);
		}
	};

	struct sphere {
		float radius;
		Vector3 center;

		sphere(Box bx)
			: radius(magnitude(bx.scale)), center(bx.center) {}

		bool isonoraboveplane(const Plane& plane) {
			return plane.distanceToPoint(center) > -radius;
		}
	};

	struct cone {
		ray dirray;
		float slope;

		cone(const ray newray, float coneslope)
			: dirray(newray), slope(coneslope) {}

		float distancefrompoint(Vector3 point) {
			// Calculate the closest point on the cone's axis
			Vector3 pointonray = dirray.pointonline(point);
			float pointdist = distance(point, pointonray);
			float slp = distance(dirray.start, pointonray);

			// Calculate the radius at the projection point
			double currentRadius = slope * slp;

			// Distance from the point to the cone's surface
			double distanceToSurface = pointdist - currentRadius;
			return distanceToSurface * slp / sqrt(currentRadius * currentRadius + slp * slp);
		}

		bool intersectssphere(sphere sph) {
			return distancefrompoint(sph.center) <= sph.radius;
		}
	};

	inline bool Box::rayintersects(ray fray) {
		Vector3 dir = fray.end - fray.start;

		float xval1 = (center.x - scale.x - fray.start.x) / dir.x;
		float xval2 = (center.x + scale.x - fray.start.x) / dir.x;
		float maxxval = fmax(xval1, xval2);
		float minxval = fmin(xval1, xval2);

		float yval1 = (center.y - scale.y - fray.start.y) / dir.y;
		float yval2 = (center.y + scale.y - fray.start.y) / dir.y;
		float maxyval = fmax(yval1, yval2);
		float minyval = fmin(yval1, yval2);

		float zval1 = (center.z - scale.z - fray.start.z) / dir.z;
		float zval2 = (center.z + scale.z - fray.start.z) / dir.z;
		float maxzval = fmax(zval1, zval2);
		float minzval = fmin(zval1, zval2);

		float actualminval = fmax(fmax(minxval, minyval), minzval);
		float actualmaxval = fmin(fmin(maxxval, maxyval), maxzval);

		return actualminval < actualmaxval && actualminval > 0;
	}

}

#endif // GEOMETRY_HPP