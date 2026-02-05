#pragma once
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <concepts>
#include "Span.h"
#include <utility>

namespace stn {

	template <typename T, size_t N>
	struct List {
		constexpr T& unchecked_at(size_t index) {
			return storage[index];
		}
		constexpr const T& unchecked_at(size_t index) const {
			return storage[index];
		}
		constexpr size_t length() const {
			return N;
		}

		constexpr T* data() {
			return storage;
		}
		constexpr const T* data() const {
			return storage;
		}
		constexpr T* begin() {
			return data();
		}
		constexpr T* end() {
			return begin() + N;
		}
		using iterator = T*;
		using const_iterator = const T*;
		constexpr const T* begin() const {
			return data();
		}
		constexpr const T* end() const {
			return begin() + N;
		}

		constexpr List() requires std::is_default_constructible_v<T> = default;

		template <typename... Args>
			requires (sizeof...(Args) == N) && (std::constructible_from<T, Args> && ...)
		constexpr List(Args... args) : storage{ std::forward<Args>(args)... } {
		}

		template <typename... Args>
			requires (sizeof...(Args) == N) &&
		(std::constructible_from<T, Args&&> && ...) &&
			((!std::is_lvalue_reference_v<Args>) && ...)
			List(Args&&... args) : storage{ std::forward<Args>(args)... } {
		}
		//apparently best aproach
	private:
		template <typename F, size_t... I>
		constexpr List(F&& f, std::index_sequence<I...>) : storage{ static_cast<T>(f(I))... } {
		}

	public:
		template<typename F>
			requires std::invocable<F&, size_t>&& std::constructible_from<T, std::invoke_result_t<F&, size_t>>
		constexpr List(F&& f) : List(std::forward<F>(f), std::make_index_sequence<N>{}) {
		}

		constexpr List(const List&) requires std::copy_constructible<T> = default;
		List& operator=(const List&) requires std::is_copy_assignable_v<T> = default;
		constexpr List(List&&) requires std::move_constructible<T> = default;
		List& operator=(List&&) requires std::is_move_assignable_v<T> = default;
		bool contains_index(size_t index) const {
			return index < N;
		}

		constexpr T& operator[](size_t index) {
			if (index >= N) {
				throw std::out_of_range("List index out of bounds");
			}
			return storage[index];
		}
		constexpr const T& operator[](size_t index) const {
			if (index >= N) {
				throw std::out_of_range("List index out of bounds");
			}
			return storage[index];
		}

		constexpr  bool operator==(const List& other) const  requires std::equality_comparable<T> {
			for (size_t i = 0; i < length(); i++) {
				if (unchecked_at(i) != other.unchecked_at(i)) {
					return false;
				}
			}
			return true;
		}
		constexpr  bool operator!=(const List& other) const requires std::equality_comparable<T> {
			return !(*this == other);
		}

		constexpr ~List() = default;

		stn::span<const T> span() const {
			return stn::span(data(), N);
		}
		stn::span<T> span() {
			return stn::span(data(), N);
		}

	private:
		T storage[N];
	};

}
