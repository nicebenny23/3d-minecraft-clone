#include <glm/mat4x4.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtc/quaternion.hpp>
#include "../math/mathutil.h"
#include <cmath>
#include <format>
#pragma once
namespace math {
	struct Angle {
		Angle() :rad(0) {

		}
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
		Angle operator-() const {
			return Angle(-rad);
		}
		Angle& operator-=(const Angle& other) {
			*this = *this - other;
			return *this;
		}
		bool operator==(const Angle& other) const{
			return math::approximate_equals(other.rad, rad);
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

	struct Look3{
		Look3(Angle yaw, Angle pitch):yaw(yaw),pitch(pitch) {

		}
		static Look3 from_degrees(double yaw, double pitch){
			return Look3(Angle::from_degrees(yaw), Angle::from_degrees(pitch));
		}
		static Look3 from_radians(double yaw, double pitch) {
			return Look3(Angle::from_radians(yaw), Angle::from_radians(pitch));
		}
		Look3() :Look3(Angle::from_degrees(90), Angle()) {

		}
		Look3 operator-() {
			return Look3(-yaw, -pitch);
		}
		Angle yaw;
		Angle pitch;

	};
}
namespace std {
	template <>
	struct formatter<math::Angle> {

		constexpr auto parse(format_parse_context& ctx) {
			return ctx.begin(); // no options
		}

		template <typename FormatContext>
		auto format(const math::Angle& a, FormatContext& ctx) const {
			return format_to(ctx.out(), "{:.2f}°", a.degrees());
		}
	}; 
	template <>
		struct formatter<math::Look3> {

		constexpr auto parse(format_parse_context& ctx) {
			return ctx.begin();
		}

		template <typename FormatContext>
		auto format(const math::Look3& n, FormatContext& ctx) const {

			double yaw = n.yaw.degrees();
			double pitch= n.pitch.degrees_signed(); 

			return format_to(
				ctx.out(),
				"(yaw={:.2f}°, pitch={:.2f}°)",
				yaw,
				pitch
			);
		}
	};
}