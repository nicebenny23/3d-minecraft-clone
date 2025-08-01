#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>

namespace util {

	template<typename T1, typename T2>
	struct pair {
		T1 first;
		T2 second;

		pair() = default;

		pair( T1&& a,  T2&& b) : first(std::move(a)), second(std::move(b)) {}
		pair(const T1& a, const T2& b) : first(a), second(b) {}
		template<std::size_t I>
		constexpr decltype(auto) at() & noexcept {
			static_assert(I < 2, "Index out of bounds");
			if constexpr (I == 0) return (first);
			else return (second);
		}

		template<std::size_t I>
		constexpr decltype(auto) at() const& noexcept {
			static_assert(I < 2, "Index out of bounds");
			if constexpr (I == 0) return (first);
			else return (second);
		}

		template<std::size_t I>
		constexpr decltype(auto) at() && noexcept {
			static_assert(I < 2, "Index out of bounds");
			if constexpr (I == 0) return std::move(first);
			else return std::move(second);
		}
		friend constexpr bool operator==(const pair& lhs, const pair& rhs) {
			return lhs.first == rhs.first && lhs.second == rhs.second;
		}

		friend constexpr bool operator!=(const pair& lhs, const pair& rhs) {
			return !(lhs == rhs);
		}
	};


	template<typename T1, typename T2>
	pair(T1, T2) -> pair<T1, T2>;


	template<std::size_t I, typename T1, typename T2>
	inline decltype(auto) get(pair<T1, T2>& p) noexcept {
		if constexpr (I == 0)
			return p.first;
		else {
			return p.second;
		}
	}

	template<std::size_t I, typename T1, typename T2>
	inline	decltype(auto) get(const pair<T1, T2>& p) noexcept {
		if constexpr (I == 0)
			return p.first;
		else
			return p.second;
	}

	template<std::size_t I, typename T1, typename T2>
	inline	decltype(auto) get(pair<T1, T2>&& p) noexcept {
		if constexpr (I == 0)
			return std::move(p.first);
		else
			return std::move(p.second);
	}

}

namespace std {

	template<typename T1, typename T2>
	struct tuple_size<util::pair<T1, T2>> : std::integral_constant<std::size_t, 2> {};

	template<typename T1, typename T2>
	struct tuple_element<0, util::pair<T1, T2>> {
		using type = T1;
	};

	template<typename T1, typename T2>
	struct tuple_element<1, util::pair<T1, T2>> {
		using type = T2;
	};

}
