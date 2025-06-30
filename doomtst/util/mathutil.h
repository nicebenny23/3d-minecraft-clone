#pragma once
#include <cmath>
#include <limits.h>
#include <stdexcept>
#include <type_traits>
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

template<typename T1, typename T2, typename... Ts>
constexpr auto Max(T1 a, T2 b, Ts... rest) -> typename std::common_type<T1, T2, Ts...>::type
{
	using Common = typename std::common_type<T1, T2, Ts...>::type;
	const Common max_ab = (a < b) ? static_cast<Common>(b) : static_cast<Common>(a);
	if constexpr (sizeof...(Ts) == 0) {
		return max_ab;
	}
	else {
		return Max(max_ab, static_cast<Common>(rest)...);
	}
}

template<typename T1, typename T2, typename... Ts>
constexpr auto Min(T1 a, T2 b, Ts... rest)-> typename std::common_type<T1, T2, Ts...>::type
{
	using Common = typename std::common_type<T1, T2, Ts...>::type;
	const Common min_ab = (a < b) ? static_cast<Common>(a) : static_cast<Common>(b);
	if constexpr (sizeof...(Ts) == 0) {
		return min_ab;
	}
	else {
		return Min(min_ab, static_cast<Common>(rest)...);
	}
}


//mod(x,m) that behaves intutiivlly (ex mod(-1,2)!=-1)
inline  int symmetric_mod(int x, int m) {
	// Ensure m is positive to avoid division by zero issues
	if (m <= 0) {
		throw std::invalid_argument("modulus cannot be negitive ");
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
//zero is mapped to 1
inline int sign(float x) {
	return (x < 0) ? -1 : 1;
}
//Zero is mapped to zero
inline int z_sign(float x) {

	return (x < 0) ? -1 : ((x > 0) ? 1 : 0);
}


int comparefloat(const void* b, const void* a);
