#include <cmath>
#include <limits.h>
#include <stdexcept>
#ifndef mathutil_HPP
#define mathutil_HPP
#define NaNf std::numeric_limits<float>::max()
inline bool aproxequal(float v1, float v2) {
	const float eps = .0001;
	return (abs(v1 - v2) < eps);

}	


inline float  interoplatequintic(const float& t) {

	return (6 * t * t - 15 * t + 10) * t * t * t;

}

//at zero val one is selected at one val2 is selected
inline float lerp(float val, float val2, float selector)
{
	return val2 * selector + val * (1 - selector);
}

inline float clamp(float val, float low, float high)
{
	if (val > high)
	{
		return high;
	}
	if (val < low)
	{
		return low;
	}
	return val;
}


inline int clamp(int val, int low, int high)
{
	if (val > high)
	{
		return high;
	}
	if (val < low)
	{
		return low;
	}
	return val;
}


inline bool inrange(float val, float low, float high)
{
	return val >= low && val <= high;
}

inline  float sigmoid(float v1) {

	return	1.0/ (1.0+ exp(-v1));
}

inline  float Max(float v1, float v2) {

	return std::fmax(v1, v2);
}
inline  float Min(float v1, float v2) {

	return std::fmin(v1, v2);

}
//mod(x,m) that behaves intutiivlly (ex mod(-1,2)!=-1)
inline  int symmetric_mod(int x, int m) {
	// Ensure m is positive to avoid division by zero issues
	if (m <= 0) {
		throw std::invalid_argument("error ");
	}
	
	// When x is negative
	if (x < 0) {
		// Calculate the modular value for negative x
		int mod = (-x % m);
		return (mod == 0) ? 0 : m - mod;
	}
	else {
		// When x is non-negative
		return x % m;
	}
}
inline  int FastFloor(float f) {
	return f >= 0 ? (int)f : (int)f - 1;
}

inline  float symmetric_floor(float x) {

	if (x < 0)
	{
		return -1 * floor(-1 * x);
	}
	else
	{
		return floor(x);
	}
}
inline int symmetric_ceil(float x) {

	if (x < 0)
	{
		return -1 * ceil(-1 * x);
	}
	else
	{
		return ceil(x);
	}
}
inline int sign(float x) {
	return (x < 0) ? -1 : 1;
}


int comparefloat(const void* b, const void* a);

#endif // !mathutil
