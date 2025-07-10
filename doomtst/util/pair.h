#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>

namespace util {

	template<typename T1, typename T2>
	struct Pair {
		T1 first;
		T2 second;

		Pair() = default;
		Pair(const T1& a, const T2& b) : first(a), second(b) {}
	};

	
	template<typename T1, typename T2>
	Pair(T1, T2) -> Pair<T1, T2>;

	
	template<std::size_t I, typename T1, typename T2>
	inline decltype(auto) get(Pair<T1, T2>& p) noexcept {
		if constexpr (I == 0)
			return p.first;
		else {
			return p.second;
		}
	}

	template<std::size_t I, typename T1, typename T2>
	inline	decltype(auto) get(const Pair<T1, T2>& p) noexcept {
		if constexpr (I == 0)
			return p.first;
		else
			return p.second;
	}

	template<std::size_t I, typename T1, typename T2>
	inline	decltype(auto) get(Pair<T1, T2>&& p) noexcept {
		if constexpr (I == 0)
			return std::move(p.first);
		else
			return std::move(p.second);
	}

}

namespace std {

	template<typename T1, typename T2>
	struct tuple_size<util::Pair<T1, T2>> : std::integral_constant<std::size_t, 2> {};

	template<typename T1, typename T2>
	struct tuple_element<0, util::Pair<T1, T2>> {
		using type = T1;
	};

	template<typename T1, typename T2>
	struct tuple_element<1, util::Pair<T1, T2>> {
		using type = T2;
	};

}
