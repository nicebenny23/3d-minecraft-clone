#include "../util/vector3.h"
#pragma once 
using namespace v3;
struct ray
{
	v3::Vec3 start;
	v3::Vec3 end;
	bool degenerate() const {
		return  dist2(start, end) == 0;
	}
	v3::Vec3 diff() const {
		return end - start;
	}
	double length() const {
		return dist(start, end);
	}
	//slower due to repitition but this code is not preformance critical
	v3::Vec3 dir() const {
		return diff() / length();
	}

	Vec3 project(v3::Vec3 vector) const {


		Vec3 aoffset = vector - start;
		if (degenerate())
		{
			return start;
		}
		Vec3 normed = dir();
		float t = dot(aoffset, normed);
		Vec3 position = start + normed * t;
		return position;
	}

	Vec3 pointAt(float t) const {
		return start + dir() * t;
	}
	double distance(v3::Vec3 vector) const {
		return dist(vector, project(vector));
	}
	ray(const v3::Vec3& startray, const v3::Vec3& endray)
	{

		start = startray;
		end = endray;

	}



	ray() = default;
};


 // !ray_hpp
