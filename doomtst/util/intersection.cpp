#include "intersection.h"
namespace geointersect {
	boxraycollision intersection(geometry::Box box, ray fray)
	{
		v3::Vector3 dir = fray.diff();

		//not actually max
		float xval1 = (box.center.x - box.scale.x - fray.start.x) / dir.x;
		float xval2 = (box.center.x + box.scale.x - fray.start.x) / dir.x;
		float maxxval = Max(xval1, xval2);
		float minxval = Min(xval1, xval2);
		float yval1 = (box.center.y - box.scale.y - fray.start.y) / dir.y;
		float yval2 = (box.center.y + box.scale.y - fray.start.y) / dir.y;
		float maxyval = Max(yval1, yval2);
		float minyval = Min(yval1, yval2);
		float zval1 = (box.center.z - box.scale.z - fray.start.z) / dir.z;
		float zval2 = (box.center.z + box.scale.z - fray.start.z) / dir.z;
		float maxzval = Max(zval1, zval2);
		float minzval = Min(zval1, zval2);
		float MaxMinValue = Max(Max(minxval, minyval), minzval);
		float MinMaxValue = Min(Min(maxxval, maxyval), maxzval);
		boxraycollision toreturn = boxraycollision();


		if (MaxMinValue < MinMaxValue)
		{
			
			if (0 < MaxMinValue)
			{

				float distancealongaxis = MaxMinValue;
				toreturn.collided = true;
				toreturn.intersectionpoint = dir * distancealongaxis + fray.start;
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