#pragma once
#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>

//contains pair types
namespace stn {
	//pair for indexes
	template<typename T>
	struct indexed {
		std::size_t index;
		T value;

		indexed() = default;

		constexpr indexed(std::size_t i, const T& v)
			: index(i), value(v) {
		}

		constexpr indexed(std::size_t i, T&& v) noexcept(std::is_nothrow_move_constructible_v<T>)
			: index(i), value(std::move(v)) {
		}
	};
	template<typename T>
	indexed(size_t, const T&) -> indexed<T>;

	// For rvalues
	template<typename T>
	indexed(size_t, T&&) -> indexed<T>;
	
	
	template<typename T>
	struct insertion {
		T value;
		bool is_new;
		
		insertion() = default;

		constexpr insertion(const T& val, bool newly_inserted)
			: value(val), is_new(newly_inserted) {
		}

		constexpr insertion(T&& val, bool newly_inserted)
			: value(std::move(val)), is_new(newly_inserted) {
		}
		constexpr explicit operator bool() const noexcept {
			return is_new;
		}
	};
	template<typename T>
	insertion(T&&,bool) -> insertion<T>;

	template<typename T>
	insertion(T&, bool) -> insertion<T>;

	template<typename T>
	insertion( const T&,bool) -> insertion<T>;

	template<std::size_t I, typename T>
	constexpr decltype(auto) get(insertion<T>& x) noexcept {
		static_assert(I < 2, "insertion<T>: index out of bounds");
		if constexpr (I == 0) return (x.value);
		else return (x.is_new);
	}

	template<std::size_t I, typename T>
	constexpr decltype(auto) get(const insertion<T>& x) noexcept {
		static_assert(I < 2, "insertion<T>: index out of bounds");
		if constexpr (I == 0) {
			return (x.value);
		}
		else {
			return (x.is_new);
		}
	}

	template<std::size_t I, typename T>
	constexpr decltype(auto) get(insertion<T>&& x) noexcept {
		static_assert(I < 2, "insertion<T>: index out of bounds");
		if constexpr (I == 0) return std::move(x.value);
		else return std::move(x.is_new);
	}


	template<typename T1, typename T2>
	struct pair {
		T1 first;
		T2 second;

		pair() = default;

		pair(T1&& a, T2&& b) : first(std::move(a)), second(std::move(b)) {}
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
	template<typename T>
	struct tuple_size<stn::indexed<T>> : std::integral_constant<std::size_t, 2> {};

	template<typename T>
	struct tuple_element<0, stn::indexed<T>> { using type = std::size_t; };

	template<typename T>
	struct tuple_element<1, stn::indexed<T>> { using type = T; };

	template<typename T>
	struct tuple_size<stn::insertion<T>> : std::integral_constant<std::size_t, 2> {};

	template<typename T>
	struct tuple_element<0, stn::insertion<T>> {
		using type = T;
	};

	template<typename T>
	struct tuple_element<1, stn::insertion<T>> {
		using type = bool;
	};
	template<typename T1, typename T2>
	struct tuple_size<stn::pair<T1, T2>> : std::integral_constant<std::size_t, 2> {};

	template<typename T1, typename T2>
	struct tuple_element<0, stn::pair<T1, T2>> {
		using type = T1;
	};

	template<typename T1, typename T2>
	struct tuple_element<1, stn::pair<T1, T2>> {
		using type = T2;
	};

}
