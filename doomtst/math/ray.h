#include "../math/vector3.h"
#include "box.h"
#pragma once 
namespace geo {
	struct ray {
		v3::Point3 start;
		v3::Point3 end;
		ray(const v3::Point3& start_ray, const v3::Point3& end_ray):start(start_ray),end(end_ray){
		}

		bool degenerate() const {
			return  v3::dist2(start, end) == 0;
		}
		v3::Vec3 diff() const {
			return end - start;
		}
		v3::Vec3 dir() const {
			if (degenerate()) {
				return v3::zerov;
			}
			return diff() / length();
		}
		Box bounding_box() const{
			return Box::from_min_max(start, end);
		}
		bool point_lies_in_sphere(v3::Point3 pnt)	const {
			return v3::dist(start, pnt) < length();
		}
		double length() const {
			return dist(start, end);
		}
		//returns a ray from its start and an offset
		static ray from_offset(v3::Point3 start, v3::Vec3 offset) {
			return ray(start, start + offset);
		}


		v3::Point3 project(v3::Point3 vector) const {


			v3::Vec3 aoffset = vector - start;
			if (degenerate()) {
				return start;
			}
			v3::Vec3 normed = dir();
			double t = dot(aoffset, normed);
			return start + normed * t;
		}
		double distance(v3::Point3 vector) const {
			return dist(vector, project(vector));
		}
		ray with_length(double length) const{
			return normalize().dialate_from_start(length);
		}
		v3::Point3 point_at(double distance) const {
			return start + dir() * distance;
		}
		v3::Point3 lerp(double t) const {
			return start + diff() * t;
		}

		ray translate(v3::Vec3 translation_vector) const {
			return ray(start + translation_vector, end + translation_vector);
		}
		ray normalize() const {
			return ray(start, start + dir());
		}

		ray dialate_from_start(double scale) const {
			return ray(start, start + diff() * scale);
		}

		ray() = default;
	};
}

 // !ray_hpp
