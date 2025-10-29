#pragma once
#include <cmath>
#include <limits.h>
#include <stdexcept>
#include <type_traits>
#include "pair.h"
#define NaNf std::numeric_limits<float>::max()
__forceinline bool apxf(double a, double b) {
	constexpr double EPS = 1e-4f;
	return (fabsf(a - b) < EPS);
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
inline bool inrange_apx(float val, float low, float high)
{
	return val >= low && val <= high||apxf(val,low)||apxf(val,high);
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

inline int next_boundary(float x, bool positiveDirection) {
	int i = static_cast<int>(std::floor(x));

	if (positiveDirection) {
		// Next boundary is always i + 1
return i + 1;
	}
	else {
		if (x==i)
		{
			return i-1;
		}
		return i;
	}
}
//
inline stn::pair<int,int> extended_range(double x) {
	int low = static_cast<int>(std::floorl(x));
	int high = low;
	int rnd = std::lround(x);
	if (apxf(rnd,x))
	{
		if (rnd==low)
		{
		low--;
		}
		else {
		high++;
		}
	}
	return stn::pair(low,high);
}
//mod(x,m) that behaves intutiivlly (ex mod(-1,2)!=-1)
__forceinline  int symmetric_mod(int x, int m) noexcept {
	// Precondition: m > 0
	int r = x % m;              // r is in (−m, +m)

	// If r is negative, shift it into [0, m) by adding m;
	// otherwise return r unchanged.
	return (r < 0) ? (r + m) : r;
} 

__forceinline float symmetric_modf(float x, float m) noexcept {
	float r = std::fmodf(x, m);
	return r < 0.0f ? r + m : r;
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


inline float wrap_to_range(float val, float low, float high) noexcept {
	return symmetric_modf(val - low, high - low) + low;
}