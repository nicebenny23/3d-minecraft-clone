#include <format>
#include "mathutil.h"
#include "vector2.h"
#include <glm/glm.hpp>
#include <string>
#include "../util/exception.h"
#include "Scale3.h"
#pragma once

namespace v3 {

	struct Vec3; // forward

	struct Vec3 {
		Vec3() : x(0), y(0), z(0) {
		}
		constexpr Vec3(double X, double Y, double Z) noexcept : x(X), y(Y), z(Z) {
		}
	
		explicit Vec3(Scale3 scale) : x(scale.x), y(scale.x), z(scale.x) {	}

		Vec3(glm::vec3 g) : x(g.x), y(g.y), z(g.z) {
		}
		bool is_nan() const {
			return isnan(x) || isnan(y) || isnan(z);
		}
		bool throw_if_nan() const {
			if (is_nan()) {
				throw std::logic_error("Vec3 Must not be NaN");
			}
		}

		void operator=(const Vec3& p1) {
			x = p1.x; y = p1.y; z = p1.z;
		}
		bool operator==(const Vec3& p1) {
			return apx(x, p1.x) && apx(y, p1.y) && apx(z, p1.z);
		}
		bool operator!=(const Vec3& p1) {
			return !apx(x, p1.x) || !apx(y, p1.y) || !apx(z, p1.z);
		}
		Vec3 operator-() const {
			return Vec3(-x,-y,-z);
		}

		Vec3 operator+(const Vec3& p1) const {
			return Vec3(x + p1.x, y + p1.y, z + p1.z);
		}
		Vec3& operator+=(const Vec3& p1) {
			x += p1.x; y += p1.y; z += p1.z; return *this;
		}

		Vec3 operator-(const Vec3& p1) const {
			return Vec3(x - p1.x, y - p1.y, z - p1.z);
		}

		Vec3& operator-=(const Vec3& p1) {
			x -= p1.x; y -= p1.y; z -= p1.z; return *this;
		}

		Vec3 operator*(double scale) const {
			return Vec3(x * scale, y * scale, z * scale);
		}

		Vec3& operator*=(double scale) {
			x *= scale; y *= scale; z *= scale; return *this;
		}

		Vec3 operator/(double scale) const {
			if (scale == 0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			return Vec3(x / scale, y / scale, z / scale);
		}

		Vec3& operator/=(double scale) {
			if (scale == 0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			x /= scale; y /= scale; z /= scale; return *this;
		}

		Vec3 operator*(const Scale3& scale) const {
			return Vec3(x * scale.x, y * scale.y, z * scale.z);
		}
		
		Vec3& operator*=(const Scale3& scale) {
			*this=*this*scale;
			return *this;
		}

		Vec3 operator/(const Scale3& inv_scale) const {
			if (inv_scale.x == 0 || inv_scale.y == 0 || inv_scale.z == 0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			return Vec3(x / inv_scale.x, y / inv_scale.y, z / inv_scale.z);
		}


		Vec3& operator/=(const Scale3& scale) {
			*this = *this/scale;
			return *this;
		}

		const double& operator[](size_t index) const {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			stn::throw_logic_error("{} is not a valid index for a Vec3",index);
			}
		}
		double& operator[](size_t index) {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			stn::throw_logic_error("{} is not a valid index for a Vec3", index);
			}
		}

		Vec3(double value, size_t index) {
			x = 0;
			y = 0;
			z = 0;
			(*this)[index] = value;
		}

		v2::Vec2 xy() const {
			return v2::Vec2(x, y);
		}
		v2::Vec2 xz() const {
			return v2::Vec2(x, z);
		}
		v2::Vec2 yz() const {
			return v2::Vec2(y, z);
		}

		glm::vec3 glm() {
			return glm::vec3(x, y, z);
		}
		double mag2() const {
			return x * x + y * y + z * z;
		}
		inline double length() const {
			return sqrt(mag2());
		}
		inline Vec3 normal() const {

			double mt = length();
			if (mt == 0) {
				throw std::logic_error("Cannot Normalize the zero vector");
			}
			return (*this / mt);

		}
		double x;
		double y;
		double z;
	};
	inline bool is_nan(v3::Vec3 vec) {
		return vec.is_nan();
	}
	inline bool throw_if_nan(v3::Vec3 vec) {
		vec.throw_if_nan();
	}
	inline constexpr Vec3 zerov{ 0.0, 0.0, 0.0 };
	inline constexpr Vec3 unitv{ 1.0, 1.0, 1.0 };

	inline constexpr Vec3 right{ 1.0, 0.0, 0.0 };
	inline constexpr Vec3 left{ -1.0, 0.0, 0.0 };

	inline constexpr Vec3 up{ 0.0, 1.0, 0.0 };
	inline constexpr Vec3 down{ 0.0, -1.0, 0.0 };

	inline constexpr Vec3 forward{ 0.0, 0.0, 1.0 };
	inline constexpr Vec3 back{ 0.0, 0.0, -1.0 };



	// free functions that use Vec3 only:
	inline double dot(const Vec3& p, const Vec3& p1) {
		return (p.x * p1.x + p.y * p1.y + p.z * p1.z);
	}
	inline double mag2(const Vec3& p) {
		return ((p.x * p.x + p.y * p.y + p.z * p.z));
	}
	inline Vec3 normal(const Vec3& p) {
		double mt = p.length();
		if (mt == 0) {
			throw std::logic_error("Cannot Normalize the zero vector");
		}
		return (p / mt);
	}
	inline Vec3 zero_fixed_normal(const Vec3& p) {
		double mt = p.length();
		if (mt == 0) {
			return zerov;
		}
		return (p / mt);
	}



	struct Point3;
	struct Coord {
		constexpr Coord(int X, int Y, int Z) noexcept : x(X), y(Y), z(Z) {
		}
		Coord() : x(0), y(0), z(0) {
		}

		explicit Coord(const Point3& p1);

		void operator=(const Coord& p1) {
			x = p1.x; y = p1.y; z = p1.z;
		}

		bool operator==(const Coord& p1) {
			return (p1.x == x && p1.y == y && p1.z == z);
		}

		bool operator!=(const Coord& p1) {
			return (p1.x != x || p1.y != y || z != p1.z);
		}

		Coord& operator+=(const Coord& p1) {
			x += p1.x; y += p1.y; z += p1.z; return *this;
		}
		Coord operator+(const Coord& p1) const {
			return Coord(x + p1.x, y + p1.y, z + p1.z);
		}
		Coord& operator-=(const Coord& p1) {
			x -= p1.x; y -= p1.y; z -= p1.z; return *this;
		}
		Coord operator-(const Coord& p1) const {
			return Coord(x - p1.x, y - p1.y, z - p1.z);
		}

		Coord operator*(int scale) const {
			return Coord(x * scale, y * scale, z * scale);
		}
		Coord& operator*=(int scale) {
			x *= scale; y *= scale; z *= scale; return *this;
		}

		Coord(int value, size_t index) {
			x = 0;
			y = 0;
			z = 0;
			(*this)[index] = value;
		}

		int& operator[](size_t index) {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			throw std::invalid_argument(std::to_string(index) + " is not a valid index for a Vec3");
			}
		}
		const int& operator[](size_t index) const {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			throw std::invalid_argument(std::to_string(index) + " is not a valid index for a Vec3");
			}
		}

		int x;
		int y;
		int z;
	};

	inline constexpr Coord RightCoord{ 1,  0,  0 };
	inline constexpr Coord LeftCoord{ -1,  0,  0 };
	inline constexpr Coord UpCoord{ 0,  1,  0 };
	inline constexpr Coord DownCoord{ 0, -1,  0 };
	inline constexpr Coord FrontCoord{ 0,  0,  1 };
	inline constexpr Coord BackCoord{ 0,  0, -1 };
	inline constexpr Coord ZeroCoord{ 0,  0,  0 };




	struct Point3 {
		Point3() : x(0), y(0), z(0) {
		}

		constexpr Point3(double X, double Y, double Z) noexcept : x(X), y(Y), z(Z){
		}

		inline Point3(Coord coord) {
			x = coord.x; y = coord.y; z = coord.z;
		}

		Point3& operator=(const Coord& p1) {
			x = p1.x; y = p1.y; z = p1.z;
			return *this;
		}

		bool is_nan() const {
			return isnan(x) || isnan(y) || isnan(z);
		}

		bool throw_if_nan() const {
			if (is_nan()) {
				throw std::logic_error("Point3 Must not be NaN");
			}
		}

		void operator=(const Point3& p1) {
			x = p1.x; y = p1.y; z = p1.z;
		}
		bool operator==(const Point3& p1) {
			return apx(x, p1.x) && apx(y, p1.y) && apx(z, p1.z);
		}
		bool operator!=(const Point3& p1) {
			return !apx(x, p1.x) || !apx(y, p1.y) || !apx(z, p1.z);
		}

		Point3 operator+(const Vec3& p1) const {
			return Point3(x + p1.x, y + p1.y, z + p1.z);
		}
		Point3& operator+=(const Vec3& p1) {
			x += p1.x; y += p1.y; z += p1.z; return *this;
		}
		
		Vec3 operator-(const Point3& p1) const {
			return Vec3(x - p1.x, y - p1.y, z - p1.z);
		}

		Point3 operator-(const Vec3& p1) const {
			return Point3(x - p1.x, y - p1.y, z - p1.z);
		}

		Point3& operator-=(const Vec3& p1) {
			x -= p1.x; y -= p1.y; z -= p1.z; return *this;
		}
		
		Point3 operator*(const Scale3& scale) const {
			return Point3(x * scale.x, y * scale.y, z*scale.z);
		}

		Point3 operator/(const Scale3& inv_scale) const {
			if (inv_scale.x == 0 || inv_scale.y == 0 || inv_scale.z == 0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			return Point3(x / inv_scale.x, y / inv_scale.y, z / inv_scale.z);
		}

		Point3 operator*(float scale) const {
			return Point3(x * scale, y * scale, z*scale);
		}
		Point3 operator/(float scale) const {
			if (scale==0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			return Point3(x/scale, y/scale, z/scale);
		}

		const double& operator[](size_t index) const {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			throw std::invalid_argument(std::to_string(index) + " is not a valid index for a Point3 ");
			}
		}
		double& operator[](size_t index) {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			throw std::invalid_argument(std::to_string(index) + " is not a valid index for a Point3 ");
			}
		}

		Point3(double value, size_t index) {
			x = 0;
			y = 0;
			z = 0;
			(*this)[index] = value;
		}

		glm::vec3 glm() const{
			return glm::vec3(x, y, z);
		}
		double x;
		double y;
		double z;
	};

	// utilities that don't require cross-definitions:

	inline double dist2(const Point3& p, const Point3& p1) {
		return mag2(p1 - p);
	}
	inline double dist(const Point3& p, const Point3& p1) {
		return (sqrt(dist2(p, p1)));
	}

	inline Vec3 yaw_pitch(double yaw, double pitch) {
		v3::Vec3 dir = v3::zerov;
		dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		dir.y = sin(glm::radians(pitch));
		dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		return normal(dir);
	}
	inline Vec3 Cross(const Vec3& p, const Vec3& p1) {
		Vec3 crosspoint;
		crosspoint.x = p.y * p1.z - p.z * p1.y;
		crosspoint.y = p.z * p1.x - p.x * p1.z;
		crosspoint.z = p.x * p1.y - p.y * p1.x;
		return crosspoint;
	}
	inline Point3 lerp(const Point3& p, const Point3& p1, double t) {
		return Point3(p.x*t+p1.x*(1-t), p.y * t + p1.y * (1 - t), p.z * t + p1.z * (1 - t));
	}
	inline Point3 operator+(const Vec3& a, const Point3& b) {
		return Point3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	inline Point3 operator+(const Point3& a, const Coord& b) {
		return Point3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	inline Point3 operator+(const Coord& a, const Point3& b) {
		return Point3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	inline Point3 operator-(const Coord& a, const Point3& b) {
		return Point3(a.x - b.x, a.y - b.y, a.z - b.z);
	}
	inline Point3 operator-(const Point3& a, const Coord& b) {
		return Point3(a.x - b.x, a.y - b.y, a.z - b.z);
	}
	inline Point3 operator+(const Vec3& a, const Coord& b) {
		return Point3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	inline Point3 operator+(const Coord& a,const Vec3& b) {
		return Point3(a.x + b.x, a.y + b.y, a.z + b.z);
	}
	inline Point3 operator-(const Coord& a, const Vec3& b) {
		return Point3(a.x - b.x, a.y - b.y, a.z - b.z);
	}


	inline Coord::Coord(const Point3& p1) {
		x = int(p1.x); y = int(p1.y); z = int(p1.z);
	}

	inline bool operator==(const Point3& p1, const Coord& p2) {
		return (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z);
	}


} // namespace v3

// formatters
template <>
struct std::formatter<v3::Vec3> : std::formatter<std::string> {
	template <typename FormatContext>
	auto format(const v3::Vec3& v, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
	}
};

template <>
struct std::formatter<v3::Coord> : std::formatter<std::string> {
	template <typename FormatContext>
	auto format(const v3::Coord& v, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({}, {}, {})", v.x, v.y, v.z);
	}
};


template <>
struct std::formatter<v3::Point3> : std::formatter<std::string> {
	template <typename FormatContext>
	auto format(const v3::Point3& v, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
	}
};
