#pragma once
#include <stdexcept>
#include <glm/vec3.hpp>
#include <string>
#include "../math/mathutil.h"
namespace v3 {
	struct Scale3 {

		constexpr Scale3() : x(1), y(1), z(1) {
		}
		constexpr Scale3(double X, double Y, double Z) noexcept : x(X), y(Y), z(Z) {
		}
		constexpr Scale3(double scale) noexcept : x(scale), y(scale), z(scale) {
		}
		void operator=(const Scale3& p1) {
			x = p1.x; y = p1.y; z = p1.z;
		}
		bool operator==(const Scale3& p1) {
			return apx(x, p1.x) && apx(y, p1.y) && apx(z, p1.z);
		}
		bool operator!=(const Scale3& p1) {
			return !apx(x, p1.x) || !apx(y, p1.y) || !apx(z, p1.z);
		}

		Scale3 operator-() const {
			return Scale3(-x, -y, -z);
		}

		constexpr Scale3 operator*(double scale) const {
			return Scale3(x * scale, y * scale, z * scale);
		}

		Scale3& operator*=(double scale) {
			*this = *this * scale;
			return *this;
		}

		Scale3 operator*(const Scale3& scale) const {
			return Scale3(x * scale.x, y * scale.y, z * scale.z);
		}

		Scale3& operator*=(const Scale3& scale) {
			*this = *this * scale;
			return *this;
		}
		Scale3 operator/(double scale) const {
			if (scale == 0) {
				throw std::logic_error("Unable to divide a Scale3 by zero");
			}
			return Scale3(x / scale, y / scale, z / scale);
		}

		Scale3& operator/=(double scale) {
			if (scale == 0) {
				throw std::logic_error("Unable to divide a Scale3 by zero");
			}
			x /= scale; y /= scale; z /= scale; return *this;
		}

		Scale3 operator/(const Scale3& inv_scale) const {
			if (inv_scale.x == 0 || inv_scale.y == 0 || inv_scale.z == 0) {
				throw std::logic_error("Unable to divide a Scale3 by zero");
			}
			return Scale3(x / inv_scale.x, y / inv_scale.y, z / inv_scale.z);
		}
		Scale3 operator/=(const Scale3& inv_scale) {
			*this = *this / inv_scale;
		}

		glm::vec3 glm() const{
			return glm::vec3(x, y, z);
		}


		const double& operator[](size_t index) const {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			throw std::invalid_argument(std::to_string(index) + " is not a valid index for a Scale3");
			}
		}
		double& operator[](size_t index) {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			throw std::invalid_argument(std::to_string(index) + " is not a valid index for a Scale3");
			}
		}

		double x;
		double y;
		double z;

	};
	inline constexpr Scale3 unit_scale{ 1.0,1.0,1.0 };
}