#include "../math/vector3.h"
#pragma once 
using namespace v3;
struct ray
{
	v3::Point3 start;
	v3::Point3 end;
	ray(const v3::Point3& startray, const v3::Point3& endray)
	{
		start = startray;
		end = endray;
	}

	bool degenerate() const {
		return  v3::dist2(start, end) == 0;
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
	static ray from_offset(v3::Point3 start, v3::Vec3 offset) {
		return ray(start, start + offset);
	}
	

	Point3 project(Point3 vector) const {


		Vec3 aoffset = vector - start;
		if (degenerate())
		{
			return start;
		}
		Vec3 normed = dir();
		double t = dot(aoffset, normed);
		return start + normed * t;
	}
	double distance(v3::Point3 vector) const {
		return dist(vector, project(vector));
	}

	Point3 pointAt(float t) const {
		return start + dir() * t;
	}
	Point3 lerp(float t) const {
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
