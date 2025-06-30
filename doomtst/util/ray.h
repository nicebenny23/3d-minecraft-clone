#include "../util/vector3.h"
#pragma once 
using namespace v3;
struct ray
{
	v3::Vector3 start;
	v3::Vector3 end;
	bool degenerate() const {
		return  dist2(start, end) == 0;
	}
	v3::Vector3 diff() const {
		return end - start;
	}
	float length() const {
		return dist(start, end);
	}
	//slower due to repitition but this code is not preformance critical
	v3::Vector3 dir() const {
		return diff() / length();
	}

	Vector3 project(v3::Vector3 vector) const {


		Vector3 aoffset = vector - start;
		if (degenerate())
		{
			return start;
		}
		Vector3 normed = dir();
		float t = dot(aoffset, normed);
		Vector3 position = start + normed * t;
		return position;
	}

	Vector3 pointAt(float t) const {
		return start + dir() * t;
	}
	float distance(v3::Vector3 vector) const {
		return dist(vector, project(vector));
	}
	ray(const v3::Vector3& startray, const v3::Vector3& endray)
	{

		start = startray;
		end = endray;

	}



	ray() = default;
};


 // !ray_hpp
