#include "util/ray.h"
#include "util/vector3.h"
using namespace v3;
#ifndef cone_HPP
#define cone_HPP
struct cone
{
	ray dirray;
	float slope;
	bool inside(Vector3 point) {

		Vector3 projection = dirray.pointonline(point);
		float startdist = distance(dirray.start, projection);
		float pointdist = distance(point, projection);
		if (pointdist/startdist>slope)
		{
			return true;
		}
		return false;
	}

	cone(const ray newray, float coneslope){
		dirray = newray;
	slope = coneslope;
	
	}
};


#endif // !cone_H
