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

		constexpr indexed(const T& v,std::size_t i)
			: index(i), value(v) {
		}

		constexpr indexed(T&& v,std::size_t i) noexcept(std::is_nothrow_move_constructible_v<T>)
			: index(i), value(std::move(v)) {
		}
	};

	struct nothing {

	};
	template<typename T>
	struct Insertion {
		T value;
		bool is_new;

		Insertion() = default;

		template<typename U = T>
			requires (!std::is_reference_v<T>&& std::constructible_from<T, U&&>)
		constexpr Insertion(U&& val, bool newly_inserted)
			: value(std::forward<U>(val)), is_new(newly_inserted) {
		}


		constexpr Insertion(T val, bool newly_inserted) noexcept requires std::is_reference_v<T>
			: value(val), is_new(newly_inserted) {
		}

		//stateful
		template<typename F>
		Insertion& on_insert(F&& f)& {
			if (is_new) {
				std::invoke(std::forward<F>(f), value);
			}
			return *this;
		}

		//stateful
		template<typename F>
		Insertion on_insert(F&& f)&& {
			if (is_new) {
				std::invoke(std::forward<F>(f), value);
			}
			return std::move(*this);
		}

		bool is_not_new() const noexcept {
			return !is_new;
		}
		constexpr explicit operator bool() const noexcept {
			return is_new;
		}
	};
	template<typename U>
	Insertion(U&&, bool) -> Insertion<std::remove_reference_t<U>>;

	template<typename U>
	Insertion(U&, bool) -> Insertion<U&>;

	template<typename U>
	Insertion(const U&, bool) -> Insertion<const U&>;

	template<std::size_t I, typename T>
	constexpr decltype(auto) get(Insertion<T>& x) noexcept {
		static_assert(I < 2, "insertion<T>: index out of bounds");
		if constexpr (I == 0) return (x.value);
		else return (x.is_new);
	}

	template<std::size_t I, typename T>
	constexpr decltype(auto) get(const Insertion<T>& x) noexcept {
		static_assert(I < 2, "insertion<T>: index out of bounds");
		if constexpr (I == 0) {
			return (x.value);
		}
		else {
			return (x.is_new);
		}
	}

	template<std::size_t I, typename T>
	constexpr decltype(auto) get(Insertion<T>&& x) noexcept {
		static_assert(I < 2, "insertion<T>: index out of bounds");
		if constexpr (I == 0) return std::move(x.value);
		else return std::move(x.is_new);
	}





}

namespace std {
	template<typename T>
	struct tuple_size<stn::indexed<T>> : std::integral_constant<std::size_t, 2> {
	};

	template<typename T>
	struct tuple_element<0, stn::indexed<T>> {
		using type = std::size_t;
	};

	template<typename T>
	struct tuple_element<1, stn::indexed<T>> {
		using type = T;
	};

	template<typename T>
	struct tuple_size<stn::Insertion<T>> : std::integral_constant<std::size_t, 2> {
	};

	template<typename T>
	struct tuple_element<0, stn::Insertion<T>> {
		using type = T;
	};

	template<typename T>
	struct tuple_element<1, stn::Insertion<T>> {
		using type = bool;
	};
	

}
