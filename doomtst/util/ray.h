#include "../util/vector3.h"
#ifndef ray_hpp
#define ray_hpp
using namespace v3;
struct ray
{
	v3::Vector3 start;
	v3::Vector3 end;
	v3::Vector3 dir() {

		return (end - start) / v3::dist(end, start);
	}
	float length() {

		return dist(start, end);
	}
inline	float distancefromray(v3::Vector3 vector) {
		Vector3 normed  = (end-start) / dist(end,start);
		Vector3 aoffset = vector-start;
		
		double t = dot(aoffset,normed);
		Vector3 position  = start +  normed*t;
		return dist(vector,position);
	}
inline	Vector3 projectpoint(v3::Vector3 vector){
	
			Vector3 normed = (end - start) / dist(end, start);
			Vector3 aoffset = vector - start;
			
			float t = dot(aoffset, normed);
			Vector3 position = start + normed * t;
			
			return position;
	}
//todo get linetraversel working
	ray(const v3::Vector3& startray, const v3::Vector3& endray)	
	{
		if (end==start)
		{
		//	Assert("end cant be equal to start in a ray");
		}
		start = startray;
		end = endray;
	}



	ray() = default;
};


#endif // !ray_hpp
