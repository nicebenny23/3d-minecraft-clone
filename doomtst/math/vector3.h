#include <format>
#include "mathutil.h"
#include "vector2.h"
#include <glm/glm.hpp>
#include <string>
#include "../util/exception.h"
#include "angle.h"
#pragma once

namespace v3 {
	struct Coord;
	//todo transfer from vector being a point in local space
	struct Vec3 {
		static Vec3 from_scale(double scale){
			return Vec3(scale, scale, scale);
		}
		
		constexpr Vec3(double X, double Y, double Z) noexcept : x(X), y(Y), z(Z) {
		}

		constexpr explicit Vec3(Coord scale);

		constexpr Vec3(glm::vec3 g) : x(g.x), y(g.y), z(g.z) {
		}
		constexpr bool is_nan() const {
			return isnan(x) || isnan(y) || isnan(z);
		}
		constexpr bool throw_if_nan() const {
			if (is_nan()) {
				throw std::logic_error("Vec3 Must not be NaN");
			}
		}

		constexpr void operator=(const Vec3& p1) {
			x = p1.x; y = p1.y; z = p1.z;
		}
		constexpr bool operator==(const Vec3& p1) {
			return math::approximate_equals(x, p1.x) && math::approximate_equals(y, p1.y) && math::approximate_equals(z, p1.z);
		}
		constexpr bool operator!=(const Vec3& p1) {
			return !math::approximate_equals(x, p1.x) || !math::approximate_equals(y, p1.y) || !math::approximate_equals(z, p1.z);
		}
		constexpr Vec3 operator-() const {
			return Vec3(-x,-y,-z);
		}
		constexpr Vec3 with_x(double value) const {
			return Vec3(value, y, z);
		}
		constexpr Vec3 with_y(double value) const {
			return Vec3(x,value, z);
		}
		constexpr Vec3 with_z(double value) const {
			return Vec3(x, y, z);
		}
		constexpr Vec3 operator+(const Vec3& p1) const {
			return Vec3(x + p1.x, y + p1.y, z + p1.z);
		}
		constexpr Vec3& operator+=(const Vec3& p1) {
			x += p1.x; y += p1.y; z += p1.z; return *this;
		}

		constexpr Vec3 operator-(const Vec3& p1) const {
			return Vec3(x - p1.x, y - p1.y, z - p1.z);
		}
		constexpr double volume() const {
			return x*y*z;
		}
		constexpr Vec3& operator-=(const Vec3& p1) {
			x -= p1.x; y -= p1.y; z -= p1.z; return *this;
		}
		constexpr Vec3 shrunk(double size) const {
			return Vec3(x - size, y - size, z - size);
		}
		constexpr Vec3 expanded(double size) const {
			return Vec3(x + size, y + size, z + size);
		}
		constexpr Vec3& shink(double size) {
			*this = shrunk(size);
			return *this;
		}
		constexpr Vec3& expand(double size) {
			*this = expanded(size);
			return *this;
		}
		constexpr Vec3 operator*(double scale) const {
			return Vec3(x * scale, y * scale, z * scale);
		}

		constexpr Vec3& operator*=(double scale) {
			x *= scale; y *= scale; z *= scale; return *this;
		}

		constexpr Vec3 operator/(double scale) const {
			if (scale == 0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			return Vec3(x / scale, y / scale, z / scale);
		}

		constexpr Vec3& operator/=(double scale) {
			if (scale == 0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			x /= scale; y /= scale; z /= scale; return *this;
		}

		constexpr Vec3 operator*(const Vec3& scale) const {
			return Vec3(x * scale.x, y * scale.y, z * scale.z);
		}
		
		constexpr Vec3& operator*=(const Vec3& scale) {
			*this=*this*scale;
			return *this;
		}

		constexpr Vec3 operator/(const Vec3& inv_scale) const {
			if (inv_scale.x == 0 || inv_scale.y == 0 || inv_scale.z == 0) {
				throw std::logic_error("Unable to divide a Vec3 by zero");
			}
			return Vec3(x / inv_scale.x, y / inv_scale.y, z / inv_scale.z);
		}


		constexpr Vec3& operator/=(const Vec3& scale) {
			*this = *this/scale;
			return *this;
		}

		constexpr const double& operator[](size_t index) const {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			stn::throw_logic_error("{} is not a valid index for a Vec3",index);
			}
		}
		constexpr double& operator[](size_t index) {
			switch (index) {
			case 0: return x;
			case 1: return y;
			case 2: return z;
			default:
			stn::throw_logic_error("{} is not a valid index for a Vec3", index);
			}
		}

		constexpr Vec3(double value, size_t index) {
			x = 0;
			y = 0;
			z = 0;
			(*this)[index] = value;
		}

		constexpr v2::Vec2 xy() const {
			return v2::Vec2(x, y);
		}
		constexpr v2::Vec2 xz() const {
			return v2::Vec2(x, z);
		}
		constexpr v2::Vec2 yz() const {
			return v2::Vec2(y, z);
		}
	

		constexpr glm::vec3 glm() {
			return glm::vec3(x, y, z);
		}
		constexpr double mag2() const {
			return x * x + y * y + z * z;
		}
		constexpr inline double length() const {
			return sqrt(mag2());
		}
		constexpr inline Vec3 normal() const {

			double mt = length();
			if (mt == 0) {
				throw std::logic_error("Cannot Normalize the zero vector");
			}
			return (*this / mt);

		}

		constexpr inline Vec3 with_length_less_than(double max_length) const {

			double mag = length();
			if (mag<=max_length) {
				return *this;
			}
			return (*this)*(max_length/mag);

		}
		constexpr inline Vec3 with_magnitude(double mag) const {
			return normal() * mag;
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



	inline Vec3 zero_fixed_normal(const Vec3& p) {
		double mt = p.length();
		if (mt == 0) {
			return zerov;
		}
		return (p / mt);
	}
	// free functions that use Vec3 only:
	inline double dot(const Vec3& p, const Vec3& p1) {
		return (p.x * p1.x + p.y * p1.y + p.z * p1.z);
	}
	inline Vec3 project(const Vec3& p, const Vec3& p1) {
		return p*v3::dot(zero_fixed_normal(p), p1);
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



	using Point3 = Vec3;
	using Scale3 = Vec3;
	inline constexpr Scale3 unit_scale{ 1.0,1.0,1.0 };
	struct Coord {
		constexpr Coord(int X, int Y, int Z) noexcept : x(X), y(Y), z(Z) {
		}
		Coord() : x(0), y(0), z(0) {
		}
		static Coord from_vec3(const Vec3& vec) {
			return Coord((int)vec.x, (int)vec.y, (int)vec.z);
		}

		inline bool operator==(const Coord& p1) const = default;
		inline bool operator!=(const Coord& p1) const = default;
		inline Coord& operator+=(const Coord& p1) {
			x += p1.x; y += p1.y; z += p1.z; return *this;
		}
		inline Coord operator+(const Coord& p1) const {
			return Coord(x + p1.x, y + p1.y, z + p1.z);
		}
		inline Coord& operator-=(const Coord& p1) {
			x -= p1.x; y -= p1.y; z -= p1.z; return *this;
		}
		inline Coord operator-(const Coord& p1) const {
			return Coord(x - p1.x, y - p1.y, z - p1.z);
		}

		inline Coord operator*(int scale) const {
			return Coord(x * scale, y * scale, z * scale);
		}
		inline Coord& operator*=(int scale) {
			x *= scale; y *= scale; z *= scale; return *this;
		}
		operator Vec3() const {
			return Vec3(x, y, z);
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
	inline size_t manhattan_distance(const Coord& p, const Coord& p1) {
		return abs(p.x-p1.x)+ abs(p.y- p1.y) + abs(p.z - p1.z);
	}
	inline constexpr Coord RightCoord{ 1,  0,  0 };
	inline constexpr Coord LeftCoord{ -1,  0,  0 };
	inline constexpr Coord UpCoord{ 0,  1,  0 };
	inline constexpr Coord DownCoord{ 0, -1,  0 };
	inline constexpr Coord FrontCoord{ 0,  0,  1 };
	inline constexpr Coord BackCoord{ 0,  0, -1 };
	inline constexpr Coord ZeroCoord{ 0,  0,  0 };



	inline constexpr v3::Vec3::Vec3(Coord scale):x(scale.x),y(scale.y),z(scale.z) {
	}
	inline double dist2(const Point3& p, const Point3& p1) {
		return mag2(p1 - p);
	}
	inline double dist(const Point3& p, const Point3& p1) {
		return (sqrt(dist2(p, p1)));
	}
	inline double manhattan_distance(const Point3& p, const Point3& p1) {
		return abs(p.x - p1.x) + abs(p.y - p1.y) + abs(p.z - p1.z);
	}

	inline Vec3 yaw_pitch(math::Look3 direction) {
		v3::Vec3 dir = v3::zerov;
		dir.x = direction.yaw.cos() * direction.pitch.cos();
		dir.y = direction.pitch.sin();
		dir.z = direction.yaw.sin() * direction.pitch.cos();
		return dir;
	}
	inline Vec3 cross(const Vec3& p, const Vec3& p1) {
		Vec3 crosspoint=v3::zerov;
		crosspoint.x = p.y * p1.z - p.z * p1.y;
		crosspoint.y = p.z * p1.x - p.x * p1.z;
		crosspoint.z = p.x * p1.y - p.y * p1.x;
		return crosspoint;
	}
	inline Point3 midpoint(const Point3& p, const Point3& p1) {
		return Point3(p + (p1 - p) * 1/2.0f);
	}
	inline Vec3 lerp(const Vec3& p, const Vec3& p1, double t) {
		return Vec3(p+(p1-p)*t);
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

#include <functional>

namespace std {
	template<>
	struct hash<v3::Coord> {
		size_t operator()(const v3::Coord& c) const noexcept {
			size_t h1 = std::hash<int>{}(c.x);
			size_t h2 = std::hash<int>{}(c.y);
			size_t h3 = std::hash<int>{}(c.z);
			size_t seed = h1;
			seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);

			return seed;
		}
	};
}