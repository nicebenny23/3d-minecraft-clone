#include "intersection.h"
namespace geointersect {
	boxraycollision intersection(geometry::Box box, ray fray)
	{
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
		boxraycollision toreturn = boxraycollision();


		if (actualminval < actualmaxval)
		{
			float closestt = actualminval;
			if (0 < actualminval)
			{


				toreturn.collided = true;
				toreturn.intersectionpoint = dir * closestt + fray.start;

				toreturn.dist = v3::dist(fray.start, toreturn.intersectionpoint);





				return toreturn;

			}

		}


		return toreturn;


	}
	bool intersects(geometry::cone cone, geometry::sphere sph)
	{
		return cone.distanceFromPoint(sph.center) <= sph.radius;

	}
}