#pragma once
#include <iterator> // for std::distance
#include <stdexcept>
#include <format>
#include <algorithm>
#include "pipeline.h"
#include <ranges>
#include <initializer_list>
#include "exception.h"
namespace stn {

	template <typename T>
	struct span {

		constexpr size_t length() const noexcept {
			return len;
		}
		constexpr size_t size() const noexcept {
			return len;
		}
		constexpr bool empty() const noexcept {
			return len == 0;
		}
		constexpr T* data() noexcept {
			return ptr;
		}
		constexpr const T* data() const noexcept {
			return ptr;
		}

		constexpr size_t data_size() const noexcept {
			return len * sizeof(T);
		}
		constexpr bool contains_index(size_t index) const noexcept {
			return index < len;
		}
		constexpr span() noexcept :ptr(nullptr), len(0) {
		};
		constexpr span(T* data, size_t length) noexcept
			: ptr(data), len(length) {
		}
		template <size_t N>
		constexpr span(T(&arr)[N]) noexcept
			: ptr(arr), len(N) {
		}
		template <std::contiguous_iterator Iter>
		span(Iter first, Iter last)
			: ptr(&(*first)), len(static_cast<size_t>(std::distance(first, last))) {

		}
		template <std::ranges::contiguous_range Container>
		span(Container& c) : span(std::begin(c), std::end(c)) {
		}
		constexpr operator stn::span<const T>() const {
			return stn::span<const T>(ptr, len);
		}
		T& operator[](size_t index) {
			if (!contains_index(index)) {
				throw_range_exception("span index {} out of range (len={})", index, len);
			}
			return ptr[index];
		}

		const T& operator[](size_t index) const {
			if (!contains_index(index)) {
				throw_range_exception("span index {} out of range (len={})", index, len);
			}
			return ptr[index];
		}
		const T& unchecked_at(size_t index) const {
			return ptr[index];
		}
		T& unchecked_at(size_t index) {
			return ptr[index];
		}

		using iterator = T*;
		using const_iterator = const T*;
		iterator begin() noexcept {
			return ptr;
		}
		const_iterator begin() const noexcept {
			return ptr;
		}
		//r
		iterator end() {
			T* d = data();
			return iterator(d ? d + len : nullptr);
		}
		const_iterator end() const {
			const T* d = data();
			return const_iterator(d ? d + len : nullptr);
		}

		template <template<typename...> class Container>
			requires std::is_constructible_v<Container<T>, iterator, iterator>
		Container<T> to() const {
			return Container<T>(begin(), end());
		}
		span from(size_t start, size_t count) const {
			//in the second important case len>start so it wont overflow
			if (len < start || len - start < count) {
				throw_range_exception("span::from requested range [{}:{}) out of bounds (valid: [0:{})", start, start + count, len);
			}
			return span<T>(ptr + start, count);
		}
		//Returns a new span that represents the range [start, end)
		span slice(size_t start, size_t end) const {
			if (start > end || end > len) {
				throw_range_exception(
					"span::slice requested range [{}:{}) out of bounds (valid: [0:{})",
					start, end, len
				);
			}
			return span(ptr + start, end - start);
		}


		//Returns a new span that represents the range [0, index)
		span first(size_t elements) const {
			if (len < elements) {
				throw_range_exception(
					"span::first requested [0:{}) out of range (valid [0:{}))",
					elements, len
				);
			}
			return span(ptr, elements);
		}
		span last(size_t elements) const {
			if (elements > len) {
				throw_range_exception(
					"span::last requested last {} elements out of range (valid [0:{}))",
					elements, len
				);
			}

			return span(ptr + (len - elements), elements);
		}
		constexpr span skip(size_t n) const {
			if (n > len) {
				throw_range_exception(
					"span::skip requested {} elements, but len is {}",
					n, len
				);
			}
			return span(ptr + n, len - n);
		}

		auto pipe()& {
			return stn::range(*this);
		}
		auto pipe()&& {
			return stn::range(std::move(*this));
		}
		template<typename Pred>
		decltype(auto) filter(Pred&& pred) const {
			return pipe().filter(pred);
		}
		template<typename Pred>
		decltype(auto) filter(Pred&& pred) {
			return pipe().filter(pred);
		}
		template<typename Function>
		decltype(auto) map(Function&& func) const {
			return pipe().map(func);
		}
		template<typename Function>
		decltype(auto) map(Function&& func) {
			return pipe().map(func);
		}
		decltype(auto) enumerate() const {
			return pipe().enumerate();
		}
		decltype(auto) enumerate() {
			return pipe().enumerate();
		}
	private:
		T* ptr;
		size_t len;
	};
	//for clarity
	template<typename T>
	concept Spanable = std::ranges::contiguous_range<T>;


}