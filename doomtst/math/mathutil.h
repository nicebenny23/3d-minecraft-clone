#pragma once
#include <cmath>
#include <limits.h>
#include <stdexcept>
#include <concepts>
#include <type_traits>
#include "../util/pair.h"
#define NaNf std::numeric_limits<double>::max()
namespace math {
	
	inline bool approximate_equals(double a, double b, double eps = 1e-4f) {
		return std::abs(a-b) < eps;
	}
	template<typename A,typename B,typename C> requires std::is_arithmetic_v<std::common_type_t<A, B, C>>
		std::common_type_t<A,B,C> clamp(const A& value, const B& low, const C& high) {
		if (value > high) {
			return high;
		}
		if (value < low) {
			return low;
		}
		return value;
	}
		inline double ease_in_power(double x, double n) {
			return std::pow(x, n);
		}

		inline double ease_out_power(double x, double n) {
			return 1 - ease_in_power(1 - x, n);
		}
		inline double ease_in_out_power(double x, double n) {
			return ease_in_power(x,n)/(ease_in_power(x,n)+ ease_in_power(1 - x, n));
		}
		inline double  interoplate_quintic(double t) {
			return (6 * t * t - 15 * t + 10) * t * t * t;

		}
	
}




inline constexpr bool in_range(double val, double low, double high) {
	return val >= low && val <= high || math::approximate_equals(val, low) || math::approximate_equals(val, high);
}
inline  double sigmoid(double v1) {

	return	1.0f / (1.0f + exp(-v1));
}
namespace stn {

	template<typename T,typename U> requires std::common_with<T,U>
	std::common_type_t<T, U> lerp(T start, U end, double selector) requires(!std::is_integral_v<T>&& !std::is_integral_v<U>){
		return start+ (end -start)* selector;
	}

	template<typename T1, typename T2, typename... Ts>
	constexpr auto max(T1 a, T2 b, Ts... rest) -> typename std::common_type<T1, T2, Ts...>::type {
		using Common = typename std::common_type<T1, T2, Ts...>::type;
		const Common max_ab = (a < b) ? static_cast<Common>(b) : static_cast<Common>(a);
		if constexpr (sizeof...(Ts) == 0) {
			return max_ab;
		}
		else {
			return max(max_ab, static_cast<Common>(rest)...);
		}
	}
	template<typename T>
	constexpr void set_max(T& value_to_set, T other) {
		value_to_set = max(value_to_set, other);
	}

	template<typename T1, typename T2, typename... Ts>
	constexpr auto min(const T1& a, const T2& b, const Ts&... rest) -> typename std::common_type<T1, T2, Ts...>::type {
		using Common = typename std::common_type<T1, T2, Ts...>::type;
		const Common min_ab = (a < b) ? static_cast<Common>(a) : static_cast<Common>(b);
		if constexpr (sizeof...(Ts) == 0) {
			return min_ab;
		}
		else {
			return min(min_ab, static_cast<Common>(rest)...);
		}
	}
	template<typename T>
	constexpr void set_min(T& value_to_set, const T& other) {
		value_to_set = min(value_to_set, other);
	}
}
namespace math {
	//if you take sign as [-1,1] this returns floor(x+sign*eps) 
	inline int floor_with_infinitesimal_shift(double x, bool sign_rounding_up) {
		int i = static_cast<int>(std::floor(x));
		//we only actually need to apply the floor 
		if (!sign_rounding_up && x == i) {
			return i - 1;
		}
		return i;

	}
	//mod(x,m) that is wrapped to [0,m)(ex mod(-3,2)=1!=-1)
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

	inline  int fast_floor(double f) {
		return f >= 0 ? (int)f : (int)f - 1;
	}
	//sign function in which 0->1
	inline int sign_rounding_up(double x) {
		return (x < 0) ? -1 : 1;
	}

	//sign function in which 0->0
	inline int zero_preserving_sign(double x) {

		return (x < 0) ? -1 : ((x > 0) ? 1 : 0);
	}


	inline double wrap_to_range(double val, double low, double high) noexcept {
		return symmetric_modf(val - low, high - low) + low;
	}
	inline double abs(double value) {
		return fabs(value);
	}
}