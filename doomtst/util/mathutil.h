#include <cmath>
#include <limits.h>
#ifndef mathutil_HPP
#define mathutil_HPP
#define NaNf std::numeric_limits<float>::max()
inline bool aproxequal(float v1, float v2) {
	float eps = .0001;
	return (abs(v1 - v2) < eps);

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
//at zero val one is selected at one val2 is selected
inline float lerp(float val, float val2, float selector)
{
	return val2 * selector + val * (1 - selector);
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
	if (val > high)
	{
		return false;
	}
	if (val < low)
	{
		return false;
	}
	return true;
}

inline bool inrangeint(int	 val, int low, int high)
{
	if (val > high)
	{
		return false;
	}
	if (val < low)
	{
		return false;
	}
	return true;
}
inline float sigmoid(float v1) {

return	exp(v1) / (1 + exp(v1));
}
inline float smoothmin(float v1,float v2) {
	return v1 + (v2 - v1) / (1 + exp((v1 - v2) / .1f));
	
}
inline float smoothmax(float v1,float v2) {

	return	exp(v1) / (1 + exp(v1));
}

inline float Max(float v1, float v2) {

	if (v1<v2)
	{
		return v2;
	}
	return v1;
}
inline float Min(float v1, float v2) {

	if (v1 < v2)
	{
		return v1;
	}
	return v2;
}inline int modabs(int x, int m) {
	// Ensure m is positive to avoid division by zero issues
	if (m <= 0) {
		return 0; // Handle invalid m by returning 0 or you could throw an exception
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
inline int FastFloor(float f) {
	return f >= 0 ? (int)f : (int)f - 1;
}

inline float floorabs(float x) {

	if (x < 0)
	{
		return -1 * floor(-1 * x);
	}
	else
	{
		return floor(x);
	}
}
inline int ceilabs(float x) {

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

	if (x < 0)
	{
		return -1;
	}
	if (0<=x)
	{
		return 1;
	}

}
#endif // !mathutil
