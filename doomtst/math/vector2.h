#pragma once 
#include <cmath>
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

#include <glm/glm.hpp>
#include <format>
namespace v2 {
	struct Vec2;
	struct Coord2 {

		constexpr Coord2(int X, int Y) noexcept : x(X), y(Y) {
		}
		constexpr Coord2() : x(0), y(0) {
		}
		bool operator==(const Coord2& p1) const {
			return (p1.x == x && p1.y == y);
		}
		bool operator!=(const Coord2& p1) const{
			return(p1.x != x || p1.y != y);
		}
		Coord2& operator+=(const Coord2& p1) {
			x += p1.x;
			y += p1.y;
			return *this;
		}
		Coord2 operator+(const Coord2& p1) const {
			return Coord2(x + p1.x, y + p1.y);
		}

		Coord2& operator-=(const Coord2& p1) {
			x -= p1.x;
			y -= p1.y;
			return *this;
		}
		Coord2 operator-(const Coord2& p1) const {
			return Coord2(x - p1.x, y - p1.y);
		}

		Coord2 operator*(int scale) const {
			return Coord2(x * scale, y * scale);
		}
		Coord2& operator*=(int scale) {
			x *= scale;
			y *= scale;
			return *this;
		}
		bool operator==(const Vec2& p1) const;
		Vec2 operator+(const Vec2& p1) const;
		Vec2 operator-(const Vec2& p1) const;

		int x;
		int y;
	};

	struct Vec2 {

		constexpr Vec2(double X, double Y) noexcept : x(X), y(Y) {
		}
		Vec2() = default;
		Vec2(glm::vec2 glm) {
			x = glm.x; y = glm.y;
		}
		explicit Vec2(Coord2 crd) {
			x = crd.x; y = crd.y;
		}
		bool operator==(const Vec2& p1) const {
			return (x == p1.x && p1.y == y);
		}

		bool operator!=(const Vec2& p1) const {
			return !(*this == p1);
		}
		Vec2& operator+=(const Vec2& p1) {
			x += p1.x;
			y += p1.y;
			return *this;
		}
		Vec2 operator+(const Vec2& p1) const {
			return Vec2(x + p1.x, y + p1.y);
		}

		Vec2& operator-=(const Vec2& p1) {
			x -= p1.x;
			y -= p1.y;
			return *this;
		}
		Vec2 operator-(const Vec2& p1) const {
			return Vec2(x - p1.x, y - p1.y);
		}

		Vec2 operator*(double scale) const {
			return Vec2(x * scale, y * scale);
		}

		Vec2& operator*=(double scale) {
			x *= scale;
			y *= scale;
			return *this;
		}
		Vec2 operator/(double scale) const {
			return Vec2(x / scale, y / scale);
		}

		Vec2& operator/=(double scale) {
			x /= scale;
			y /= scale;
			return *this;
		}
		bool operator==(const Coord2& p1) const {
			return (p1.x == x && p1.y == y);
		}
		double x;
		double y;
		void operator= (const Coord2& p1) {
			x = p1.x;
			y = p1.y;
		}
		Vec2 operator*(Vec2 scale) const {
			return Vec2(x * scale.x, y * scale.y);
		}
	};

	const  Vec2 zerov = Vec2(0, 0);
	const  Vec2 unitv = Vec2(1, 1);

	inline Vec2 Coord2::operator+(const Vec2& p1) const {
		return Vec2(p1.x + x, p1.y + y);
	}

	inline Vec2 Coord2::operator-(const Vec2& p1) const {
		return Vec2(x - p1.x, y - p1.y);
	}

	inline bool Coord2::operator==(const Vec2& p1) const {
		return (p1.x == x && p1.y == y);
	}

	inline double dist(const Vec2& p, const Vec2& p1) {
		return(sqrt((p.x - p1.x) * (p.x - p1.x) + (p.y - p1.y) * (p.y - p1.y)));
	}

	inline double len(const Vec2& p) {
		return(sqrt(p.x * p.x + p.y * p.y));
	}
	inline Vec2 normal(Vec2 p) {
		if (p == zerov) {
			return zerov;
		}
		return(p / len(p));
	}

	inline double dot(const Vec2& p, const Vec2& p1) {
		return (p.x * p1.x + p.y + p1.y);
	}

	inline Vec2 lerp(const Vec2& p, const Vec2& p1, double t) {
		return p * (1 - t) + p1 * t;
	}

	inline double slope(const Vec2& p, const Vec2& p1) {
		return (p1.y - p.y) / (p1.x - p.x);
	}
}
template <>
struct std::formatter<v2::Coord2> : std::formatter<std::string> {
	template <typename FormatContext>
	auto format(const v2::Coord2& v, FormatContext& ctx) const {
		return std::format_to(ctx.out(), "({}, {})", v.x, v.y);
	}
};
