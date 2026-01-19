#include "mathutil.h"
#pragma once
namespace math {
	//closed interval
	struct range {
		range(double minimum, double maximum) :min(minimum), max(maximum) {

			if (max < min) {
				std::swap(min, max);
			}
		}
		range operator+(const range& oth)const {
			return range(min + oth.min, max + oth.max);
		}
		range& operator+=(const range& oth) {
			min += oth.min;
			max += oth.max;
			return *this;
		}
		range operator*(double scale) const{
			if (scale < 0) {
				return range(max * scale, min * scale);
			}
			else {
				return range(min * scale, max * scale);
			}
		}
		range& operator*=(double scale) {
			if (scale < 0) {
				*this = range(max * scale, min * scale);
			}
			else {
				*this = range(min * scale, max * scale);
			}
			return *this;
		}
		double length() const {

			return max - min;
		}

		double lower() const {
			return min;
		}
		double upper() const {
			return max;
		}
		bool contains(double val) const {
			return inrange(val, min, max);
		}
		bool apx_contains(double val) const {
			return inrange_apx(val, min, max);
		}
		bool contains(const range& other) const{
			return contains(other.max) && contains(other.min);
		}

	private:
		double min, max;
	};
}