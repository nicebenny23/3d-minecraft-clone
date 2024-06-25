#include "vector3.h"
#ifndef ray_hpp
#define ray_hpp

struct ray
{
	v3::Vector3 start;
	v3::Vector3 end;
	v3::Vector3 dir() {

		return (end - start) / v3::distance(end, start);
	}
	float distancefromray(v3::Vector3 vector) {
		return 0;
	}

	ray(const v3::Vector3& startray, const v3::Vector3& endray)
		
	{
		start = startray;
		end = endray;
	}


};


#endif // !ray_hpp
