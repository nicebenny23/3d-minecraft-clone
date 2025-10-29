#include "../util/vector3.h"
#pragma once 
using namespace v3;
struct ray
{
	v3::Vec3 start;
	v3::Vec3 end;
	ray(const v3::Vec3& startray, const v3::Vec3& endray)
	{
		start = startray;
		end = endray;
	}

	bool degenerate() const {
		return  dist2(start, end) == 0;
	}
	v3::Vec3 diff() const {
		return end - start;
	}
	v3::Vec3 dir() const {
		return diff() / length();
	}

	double length() const {
		return dist(start, end);
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
	double distance(v3::Vec3 vector) const {
		return dist(vector, project(vector));
	}

	Vec3 pointAt(float t) const {
		return start + dir() * t;
	}
	Vec3 lerp(float t) const {
		return start + diff() * t;
	}

	ray translate(Vec3 translation_vector) const {
		return ray(start + translation_vector, end + translation_vector);
	}
	ray normalize() const{
		return ray(start ,start+ dir());
	}
	
	ray dialate_from_start(float scale) const {
		return ray(start, start +diff()*scale);
	}

	ray() = default;
};


 // !ray_hpp
