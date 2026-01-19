#pragma once
#include <cmath>
#include <limits.h>
#include <stdexcept>
#include <type_traits>
#include "../util/pair.h"
#define NaNf std::numeric_limits<double>::max()
__forceinline bool apx(double a, double b) {
	constexpr double EPS = 1e-4f;
	return (abs(a - b) < EPS);
}


inline double  interoplate_quintic(double t) {

	return (6 * t * t - 15 * t + 10) * t * t * t;

}

//at zero val one is selected at one val2 is selected

inline double lerp(double val, double val2, double selector)
{
	return val2 * selector + val * (1 - selector);
}

inline double clamp(double val, double low, double high)
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


inline bool inrange(double val, double low, double high)
{
	return val >= low && val <= high;
}
inline bool inrange_apx(double val, double low, double high)
{
	return val >= low && val <= high||apx(val,low)||apx(val,high);
}

inline  double sigmoid(double v1) {

	return	1.0f/ (1.0f+ exp(-v1));
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

inline int next_boundary(double x, bool positiveDirection) {
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
	if (apx(rnd,x))
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
	
	int r = x % m;              // r is in (−m, +m)

	// If r is negative, shift it into [0, m) by adding m;
	// otherwise return r unchanged.
	return (r < 0) ? (r + m) : r;
} 

__forceinline double symmetric_modf(double x, double m) noexcept {
	double r = std::fmod(x, m);
	return r < 0.0f ? r + m : r;
}

inline  int FastFloor(double f) {
	return f >= 0 ? (int)f : (int)f - 1;
}

inline  double symmetric_floor(double x) {

	if (x < 0)
	{
		return -1 * floor(-1 * x);
	}
	else
	{
		return floor(x);
	}
}
inline int symmetric_ceil(double x) {

	if (x < 0)
	{
		return int(- 1 * ceil(-1 * x));
	}
	else
	{
		return int(ceil(x));
	}
}

inline int sign(double x) {
	return (x < 0) ? -1 : 1;
}

//Zero is mapped to zero
inline int z_sign(double x) {

	return (x < 0) ? -1 : ((x > 0) ? 1 : 0);
}


inline double wrap_to_range(double val, double low, double high) noexcept {
	return symmetric_modf(val - low, high - low) + low;
}