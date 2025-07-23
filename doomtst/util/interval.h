#include "mathutil.h"
#pragma once
namespace inter {
	//closed interval
	struct range {
		range() :min(0), max(0) {};
		range(float mn, float mx):min(mn),max(mx) {

			if (max<min)
			{
				std::swap(min, max);
			}
		}
		range operator+(const range& oth) {
			return range(min + oth.min, max + oth.max);
		}
		range& operator+=(const range& oth) {
			min += oth.min;
			max += oth.max;
			return *this;
		}
		range operator*(float scale) {
			if (scale<0)
			{
				return range(max * scale, min * scale);
			}else
			{
				return range(min* scale, max* scale);
			}
		}
		range& operator*=(float scale) {
			if (scale < 0)
			{
				*this = range(max * scale, min * scale);
			}
			else
			{
				*this = range(min * scale, max * scale);
			}
			return *this;
		}
		float length() {

			return max - min;
		}
		float lower() {
			return min;
		}
		float upper() {
			return max;
		}
		bool contains(float val) {
			return inrange(val,min, max);
		}
		bool contains(const range& other) {
			return contains(other.max)&&contains(other.min);
		}
	
	private:
		float min,max;
	};
}