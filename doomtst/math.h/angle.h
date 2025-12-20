#include <glm/mat4x4.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtc/quaternion.hpp>
#include "../util/mathutil.h"
#include <cmath>
#pragma once
namespace math {
	struct Angle {
		double cos() const {
			return glm::cos(rad);
		}
		double sin() const {
			return glm::sin(rad);
		}
		Angle operator+(const Angle& other) const{
			return Angle(rad + other.rad);
		}
		Angle& operator+=(const Angle& other) {
			*this = *this + other;
			return *this;
		}
		Angle operator-(const Angle& other) const {
			return Angle(rad - other.rad);
		}
		Angle& operator-=(const Angle& other) {
			*this = *this - other;
			return *this;
		}
		bool operator==(const Angle& other) const{
			return apx(other.rad, rad);
		}
		bool operator!=(const Angle& other) const {
			return !(*this==other);
		}
		static Angle from_degrees(double degrees) {
			return Angle(degrees * glm::pi<double>() / 180.0);
		}
		static Angle from_radians(double radians) {
			return Angle(radians);
		}
		double radians() const {
			return rad;
		}
		double degrees() const {
			return rad * 180.0 / glm::pi<double>();
		}
		double radians_signed() const {
			double r = rad;
			if (r > glm::pi<double>()) r -= 2.0 * glm::pi<double>();
			return r;
		}

		double degrees_signed() const {
			double d = rad * 180.0 / glm::pi<double>();
			if (d > 180.0) d -= 360.0;
			return d;
		}
	private:

		Angle(double radians) {
				rad = std::fmod(radians, 2.0 * glm::pi<double>());
				if (rad < 0) rad += 2.0 * glm::pi<double>();
		}
		double rad;
	};

	struct YawPitch{

		Angle yaw;
		Angle pitch;

	};
}