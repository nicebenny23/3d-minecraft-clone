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
			double currentRadius =double( slope * slp);

			// Distance from the point to the cone's surface
			double distanceToSurface = pointdist - currentRadius;
			return distanceToSurface * slp / sqrt(currentRadius * currentRadius + slp * slp);
		}

		bool intersectssphere(sphere sph) {
			return distancefrompoint(sph.center) <= sph.radius;
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

	inline  v3::Vector3 intersectionpoint (Box box,ray fray) {
		
			v3::Vector3 dir = fray.end - fray.start;

			//not actually max
			float xval1 = (box.center.x - box.scale.x - fray.start.x) / dir.x;
			float xval2 = (box.center.x + box.scale.x - fray.start.x) / dir.x;
			float maxxval = fmax(xval1, xval2);
			float minxval = fmin(xval1, xval2);
			float yval1 = (box.center.y - box.scale.y - fray.start.y) / dir.y;
			float yval2 = (box.center.y + box.scale.y - fray.start.y) / dir.y;
			float maxyval = fmax(yval1, yval2);
			float minyval = fmin(yval1, yval2);
			float zval1 = (box.center.z - box.scale.z - fray.start.z) / dir.z;
			float zval2 = (box.center.z + box.scale.z - fray.start.z) / dir.z;
			float maxzval = fmax(zval1, zval2);
			float minzval = fmin(zval1, zval2);
			float actualminval = fmax(fmax(minxval, minyval), minzval);
			float actualmaxval = fmin(fmin(maxxval, maxyval), maxzval);

			if (actualminval < actualmaxval)
			{
				float closestt = actualminval;
				if (0 < actualminval)
				{
				return dir* closestt + fray.start;

				}

			}
			

			return Vector3(NaNf,NaNf,NaNf);

		}
	

}

#endif // GEOMETRY_HPP