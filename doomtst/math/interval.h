#include "mathutil.h"
#include <utility>
#pragma once
namespace math {
	//closed interval
	struct bounds {

		constexpr bounds(double min, double max) :minimum(min), maximum(max) {
			if (maximum < minimum) {
				std::swap(minimum, maximum);
			}
		}
		static constexpr bounds unchecked_bounds(double min,double max) {
			return bounds(min, max, unchecked_tag{});
		}
		  
		static constexpr bounds from_midpoint_length(double midpoint, double length) {
			return bounds(midpoint - length/2, midpoint + length/2);
		}
		static constexpr bounds from_center_radius(double center, double radius) {
			return bounds(center-radius, center+radius);
		}
		constexpr bounds operator+(const bounds& oth)const {
			return unchecked_bounds(minimum + oth.minimum, maximum + oth.maximum);
		}
		constexpr bounds operator-()const {
			return unchecked_bounds(-maximum,minimum);
		}
		constexpr bounds& operator+=(const bounds& oth) {
			minimum += oth.minimum;
			maximum += oth.maximum;
			return *this;
		}
		
		constexpr bounds operator*(double scale) const{
			return bounds(maximum * scale, minimum * scale);
		}
		constexpr bounds& operator*=(double scale) {
			*this = (*this) * scale;
			return *this;
		}

		constexpr bounds operator-(const bounds& oth) const {
			return *this+(-oth);
		}
		constexpr bounds& operator-=(const bounds& oth) {
			*this = *this - oth;
			return *this;
		}
		
		constexpr bounds operator/(double scale) const {
			return *this * (1 / scale);
		}

		constexpr bounds& operator/=(double scale) {
			return *this *= (1/scale);
		}
		constexpr double length() const {
			return maximum - minimum;
		}
		constexpr double midpoint() const {
			return (maximum + minimum)/ 2;
		}
		constexpr double min() const {
			return minimum;
		}
		constexpr double max() const {
			return maximum;
		}
		constexpr double lerp(double selector) const{
			return minimum+ length()* selector;
		}
		double distance_to(double value) const{
			return abs(clamp(value) - value);
		}
		double signed_distance_to(double value) const {
			if (contains(value)) {
				return -stn::min(value - minimum, maximum - value);
			}
			return distance_to(value);
		}
		constexpr double unlerp(double result) const {
			double len = length();
			if (len == 0) {
				return 0;
			}
			return (result-minimum)/len;
		}
		constexpr double unlerp_clamped(double result) const {
			return unlerp(clamp(result));
		}
		
		constexpr bool contains(double val) const {
			return in_range(val, minimum, maximum);
		}

		constexpr bool contains_exact(double val) const {
			return minimum<=val&&val<=maximum;
		}
		constexpr bool contains(const bounds& other) const{
			return contains(other.maximum) && contains(other.minimum);
		}
		constexpr double clamp(double value) const {
			if (value < minimum) {
				return minimum;
			}
			if (value > maximum) {
				return maximum;
			}
			return value;
		}
		stn::Option<bounds> intersect(const bounds& other) const {
			double new_min = stn::max(minimum, other.minimum);
			double new_max = stn::min(maximum, other.maximum);
			if (new_max<new_min) {
				return stn::None;
			}
			return bounds::unchecked_bounds(new_min, new_max);
		}
		bool intersects(const bounds& other) const {
			return intersect(other).is_some();
		}

		constexpr bounds unite(const bounds& other) const {
			return bounds(
				stn::min(minimum, other.minimum),
				stn::max(maximum, other.maximum)
			);
		}
	private:
		struct unchecked_tag {

		};
		constexpr bounds(double min, double max, unchecked_tag tag) :minimum(min), maximum(max) {

		}
		double minimum, maximum;
	}; 

	inline constexpr bounds all_reals= bounds::unchecked_bounds(-std::numeric_limits<double>::infinity(),
		std::numeric_limits<double>::infinity());
	inline constexpr bounds unit_bounds = bounds::unchecked_bounds(0.0, 1.0);

	inline constexpr bounds centered_unit_bounds = bounds::unchecked_bounds(-.5f,.5f);
}