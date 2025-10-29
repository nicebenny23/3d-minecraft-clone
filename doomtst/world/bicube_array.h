#include "../util/List.h"
#include <bit>
#pragma once
namespace bicube_array {
	constexpr bool is_pow_2(size_t n) {
		return n > 0 && std::has_single_bit(n);
	}
	constexpr std::size_t bin_log(std::size_t n) {
		if (n == 0) {
			throw "bin_log requires positive input";
		}
		return std::bit_width(n) - 1;
	}
	template <std::size_t n>
	concept is_pow_2_c = is_pow_2(n);
	template<typename T,size_t size> requires is_pow_2_c<size>
	struct bicube {
		inline constexpr size_t total_size = size * size * size;
		inline constexpr size_t width = bin_log(size);
		stn::List<T, total_size> list;
		T& operator[](size_t x, size_t y, size_t z) {
		return list[x + y <<width + z << (2*width)];
		}
		const T& operator[](size_t x, size_t y, size_t z) {
			return list[x + y << width + z << (2 * width)];
		}
		using iterator = decltype(list)::iterator;
		iterator begin()
		{
			return list.begin();
		}
		iterator end()
		{
			return list.end();
		}
	};


}