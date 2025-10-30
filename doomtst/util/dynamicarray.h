#pragma once

#include <stdexcept>
#include <vector>
#include <new>
#include <cstdlib>
#include <cstring>
#include "span.h"
#include "Option.h"
#include "array_storage.h"
#include "pipeline.h"
namespace stn {
	//make a std::out_of_range formatted exepction

	// Resizing function that doubles the size of the array when needed.
	// If the length is 0, it initializes to a default size of 2.
	inline constexpr size_t resize_length(size_t len) {
		return (len == 0) ? 2 : 2 * len;  // Double the capacity for efficient reallocation
	}
	template<class T>
	class array {
	public:

		static constexpr bool is_default = std::is_default_constructible_v<T>;
		static constexpr bool is_trivially_copyable = std::is_trivially_copyable_v<T>;
		static constexpr bool is_copyable = std::is_copy_assignable_v<T>;
		inline constexpr bool empty() const
		{
			return len == 0;
		}
		inline constexpr std::uint32_t cap() const
		{
			return capacity;
		}
		inline constexpr std::uint32_t length() const
		{
			return len;
		}
		inline constexpr std::uint32_t size() const {
			return len;
		}

		constexpr bool contains_index(uint32_t index) const {

			return index < len;
		}

		constexpr T* data() {
			return list.data();
		}
		constexpr const T* data() const {
			return list.data();
		}

		stn::span<T> span() {
			return stn::span<T>(data(), len);
		}
		stn::span<const T> span() const {
			return stn::span<const T>(data(), len);
		}


		decltype(auto) pipe() const& {
			return stn::range(*this);
		}
		decltype(auto) pipe()& {

			return stn::range(*this);
		}
		decltype(auto) pipe()&& {
			return stn::range(std::move(*this));
		}
		[[nodiscard]] T& unchecked_at(uint32_t index) {
			return list[index];
		}
		[[nodiscard]] const T& unchecked_at(uint32_t index) const {
			return list[index];
		}
		[[nodiscard]] T& operator[](uint32_t index) {
			if (!contains_index(index)) {
				throw make_range_exception("Index {} is out of bounds: cannot access element in array of length {}", index, length());
			}
			return list[index];
		}
		[[nodiscard]] const T& operator[](uint32_t index) const {
			if (!contains_index(index)) {
				throw make_range_exception("Index {} is out of bounds: cannot access element in array of length {}", index, length());
			}
			return list[index];
		}
		[[nodiscard]] T& reach(uint32_t index) requires(is_default)
		{
			if (!contains_index(index))
			{
				geometric_expand(index + 1);
			}
			return list[index];
		}
		[[nodiscard]] T& first()
		{
			if (empty())
			{
				throw std::logic_error("Cannot access the first element of an empty array");
			}
			return list[0];
		}
		[[nodiscard]] const T& first() const
		{
			if (empty())
			{
				throw std::logic_error("Cannot access the first element of an empty array");
			}
			return list[0];
		}
		[[nodiscard]] T& last()
		{
			if (empty())
			{
				throw std::logic_error("Cannot access the last element of an empty array");
			}
			return list[len - 1];
		}
		[[nodiscard]] const T& last() const
		{
			if (empty())
			{
				throw std::logic_error("Cannot access the last element of an empty array");
			}
			return list[len - 1];
		}


		// for_each using C++20 ranges
		template <std::ranges::forward_range Range, typename Func>
		void for_each(Func&& fn) {
			for (size_t i = 0; i < len; i++)
			{
				fn(list[i]);
			}
		}
		Option<uint32_t> find(const T& value) const {
			for (size_t i = 0; i < len; i++)
			{
				if (list[i] == value)
				{
					return Option<uint32_t>(i);
				}
			}
			return None;
		}
		// contains using ranges
		bool contains(const T& value) const {
			return find(value).is_some();
		}
		template<typename Pred>
		bool any(Pred&& pred) {
			return pipe().any(pred);
		}

		template<typename Pred>
		bool all(Pred&& pred) const {
			return pipe().all(pred);
		}
		template<typename Pred>
		decltype(auto) filter(Pred&& pred) const {
			return pipe().filter(pred);
		}
		template<typename Function>
		decltype(auto) map(Function&& func) const {
			return pipe().map(func);
		}
		template<typename Function>
		decltype(auto) enumerate() const {
			return pipe().enumerate();
		}

		template<typename... Args>
			requires std::constructible_from<T, Args&&...>
		void emplace(Args&&... args) {
			geometric_reserve(len + 1);
			list.construct_at(len++, std::forward<Args>(args)...);
		}

		template<typename U>
		void push(U&& value) requires std::constructible_from<T,U&&> {
			geometric_reserve(len + 1);
			list.construct_at(len++, std::forward<U>(value));
		}
		
		template<typename U>
		void push(std::initializer_list<U> ilist) requires std::convertible_to<U, T> {
			geometric_reserve(len + ilist.size());
			for (auto&& value : ilist) {
				list.construct_at(len++, value);
			}
		}
		template <std::ranges::forward_range Container>
		void append(Container&& range) {
			size_t otherlen = std::ranges::distance(range);
			geometric_reserve(len + otherlen);
			if constexpr (std::ranges::contiguous_range<Container>) {

				if (use_mem_ops())
				{
					std::memmove(&list[len], std::data(range), otherlen * sizeof(T));
					len += otherlen;
					return;
				}
			}
			for (const T& value : range)
			{
				list.construct_at((len++), std::forward<decltype(value)>(value));
			};
		}
		
		template<typename... Args>
		T& emplace_at(size_t index, Args&&... args) 	requires std::constructible_from<T, Args&&...> {
			if (index > len) {
				throw make_range_exception("Emplace failed: index {} out of bounds (length {})", index, len);
			}

			geometric_reserve(len + 1);

			if (use_mem_ops()) {
				if (index != len) {
					std::memmove(&list[index + 1], &list[index], (len - index) * sizeof(T));
				}
				list.construct_at(index, std::forward<Args>(args)...);
			}
			else {
				if (index != len) {
					list.construct_at(len, std::move(list[len - 1]));
					for (size_t i = len - 1; i > index; --i) {
						list[i] = std::move(list[i - 1]);
					}
					list.destruct_at(index);
				
				}
				list.construct_at(index, std::forward<Args>(args)...);
			}

			++len;
			return list[index];
		}
		template<typename U>
		void insert_at(size_t index, U&& value) requires std::convertible_to<U&&, T> {
			emplace_at(index, std::forward<U>(value));
		}
		//removes and returns the last element(make nothrow move asignable
		[[nodiscard]] T pop() {
			if (empty())
			{
				throw std::logic_error("Cannot pop from empty array");
			}
			//subtracts first to get the last element;
			T res = std::move(list[(--len)]);
			list.destruct_at(len);
			return res;
		}
		//removes the last element
		void drop() {
			if (empty())
			{
				throw std::logic_error("Cannot drop from empty array");
			}
			list.destruct_at(--len);
		}
		void swap_drop(uint32_t index) {
			if (!contains_index(index)) {
				throw make_range_exception(
					"swap_drop failed: index {} out of bounds (length {})", index, len);
			}


			if (index != len - 1) {
				list[index] = std::move(list[len - 1]); // move last element to 'hole'
			}

			list.destruct_at(len - 1); // destroy old last element
			len--;
		}
		// Delete an element at the given index, shifting others.
		void remove_at(uint32_t index) {
			if (!contains_index(index))
			{
				throw make_range_exception("Deletion failed: index {} out of bounds (length {})", index, len);
			}
			//works by backtracking all elements above it overiding them
			len--;

			if (use_mem_ops()) {
				if (index < len) {
					std::memmove(&list[index], &list[index + 1], (len - index) * sizeof(T));
				}
			}
			else {
				for (size_t i = index; i < len; ++i) {
					list[i] = std::move(list[i + 1]);
				}
			}
			list.destruct_at(len);
		}

		[[nodiscard]] bool operator==(const array& other) const
		{
			if (len != other.len)
			{
				return false;
			}
			for (size_t i = 0; i < len; i++)
			{
				if (other.list[i] != list[i])
				{
					return false;
				}
			}
			return true;
		}
		[[nodiscard]] bool operator!=(const array& other) const
		{
			return !(*this == other);
		}

		void swap(array& other) {
			std::swap(len, other.len);
			std::swap(capacity, other.capacity);
			list.swap(other.list);
		}


		constexpr array() noexcept :len(0), capacity(0), list() {}

		array(uint32_t length, const T& default_value)  requires (is_copyable) {

			len = length;
			capacity = 0;
			geometric_reserve(len);
			for (size_t i = 0; i < len; i++)
			{
				list.construct_at(i, default_value);
			}
		}
		array(uint32_t length)  requires(is_default) {
			len = length;
			capacity = 0;
			geometric_reserve(len);
			for (size_t i = 0; i < len; i++)
			{
				list.construct_at(i, T());
			}
		}
		
		// Copy constructor.
		explicit array(const array& arr) requires (is_copyable) {

			len = 0;
			capacity = 0;
			reserve(arr.len);
			len = arr.len;
			//copy optimization

			if (use_mem_ops())
			{
				list.mem_copy_from(arr.list, arr.len);
			}
			else {
				for (size_t i = 0; i < len; i++) {
					list.construct_at(i, arr.list[i]);
				}
			}
		}
		array& operator=(const array& other) requires(is_copyable) {
			reserve(other.len);

			if (use_mem_ops()) {

				list.mem_copy_from(other.list, other.len);
			}
			else {
				for (size_t i = 0; i < len; i++) {
					list.expand_at(i, other.list[i]);
				}
				for (size_t i = len; i < other.len; i++) {
					list.construct_at(i, other.list[i]);
				}
				for (size_t i = other.len; i < len; i++) {
					list.destruct_at(i);
				}
			}
			len = other.len;
			return *this;
		}
		array(array&& other) noexcept
		{
			len = other.len;
			capacity = other.capacity;
			list = std::move(other.list);
			other.len = 0;
			other.capacity = 0;

		}
		array& operator=(array&& other) = default;

		template<std::forward_iterator InputIt>
		array(InputIt first, InputIt last) : len(0), capacity(0), list() {
			// Count elements if possible
			uint32_t length = std::distance(first, last);
			if (length != 0) {
				reserve(length);

				uint32_t i = 0;
				for (; first != last; ++first, ++i) {
					// move if possible, otherwise copy
					list.construct_at(i, std::forward<decltype(*first)>(*first));
				}

				len = length;
			}
		}
		template<std::ranges::forward_range Range>
		array(Range&& Rng) : array(Rng.begin(), Rng.end()) {}
		template<typename U> requires std::convertible_to<U,T>
		array(std::initializer_list<U> ilist) : array(ilist.begin(), ilist.end()) {}
		void clear()
		{
			if (!empty()) {
				if constexpr (!std::is_trivially_destructible_v<T>)
				{
					for (size_t i = 0; i < len; i++)
					{
						list.destruct_at(i);
					}
				}
				list.reset();
				len = 0;
				capacity = 0;
			}
		}

		~array() {
			clear();
		}


		template <typename U>
		class iterator_base {
		public:
			using iterator_category = std::contiguous_iterator_tag;
			using value_type = U;
			using difference_type = std::ptrdiff_t;
			using pointer = U*;
			using reference = U&;
			iterator_base(pointer p) : ptr(p) {}
			iterator_base() : ptr(nullptr) {}
			reference operator*() const { return *ptr; }
			pointer operator->() const { return ptr; }
			iterator_base& operator++() { ++ptr; return *this; }
			iterator_base operator++(int) { iterator_base tmp = *this; ++(*this); return tmp; }
			iterator_base& operator--() { --ptr; return *this; }
			iterator_base operator--(int) { iterator_base tmp = *this; --(*this); return tmp; }
			bool operator!=(const iterator_base& other) const { return ptr != other.ptr; }
			bool operator==(const iterator_base& other) const { return ptr == other.ptr; }
			friend iterator_base operator+(difference_type n, const iterator_base& it) {
				return iterator_base(it.ptr + n);
			}
			iterator_base operator+(difference_type n) const { return iterator_base(ptr + n); }
			iterator_base operator-(difference_type n) const { return iterator_base(ptr - n); }
			difference_type operator-(const iterator_base& other) const { return ptr - other.ptr; }
			iterator_base& operator+=(difference_type n) { ptr += n; return *this; }
			iterator_base& operator-=(difference_type n) { ptr -= n; return *this; }
			reference operator[](difference_type n) const { return ptr[n]; }
			bool operator<(const iterator_base& other) const { return ptr < other.ptr; }
			bool operator>(const iterator_base& other) const { return ptr > other.ptr; }
			bool operator<=(const iterator_base& other) const { return ptr <= other.ptr; }
			bool operator>=(const iterator_base& other) const { return ptr >= other.ptr; }
			friend constexpr pointer to_address(const iterator_base& it) noexcept {
				return it.ptr;
			}
		private:
			pointer ptr;
		};
		decltype(auto) drain() {
			return stn::drain(std::move(*this));
		}
		using iterator = iterator_base<T>;
		using const_iterator = iterator_base<const T>;
		iterator begin() { return iterator(data()); }
		const_iterator begin() const { return const_iterator(data()); }
		//because nullptr addition is ub
		iterator end() {
			T* d = data();
			return iterator(d ? d + len : nullptr);
		}
		const_iterator end() const {
			const T* d = data();
			return const_iterator(d ? d + len : nullptr);
		}
		//expands the length and capacity to fit size exactly
		void expand(uint32_t size) requires(is_default) {
			if (len < size)
			{
				reserve(size);
				for (uint32_t i = len; i < size; i++) {
					list.construct_at(i, T());
				}
				len = size;
			}

		}
		void geometric_expand(uint32_t size) requires(is_default) {
			if (len < size)
			{
				reserve(resize_length(size));
				for (size_t i = len; i < size; i++) {
					list.construct_at(i, T());
				}
				len = size;
			}
		}

		//expands the capacity geometricly
		void geometric_reserve(uint32_t size) {
			if (capacity < size)
			{
				reserve(resize_length(size));
			}

		}

		void reserve(uint32_t size) {
			if (size <= capacity)
			{
				return;
			}
			// arbitrary safety margin to prevent overflow errors 
			constexpr uint32_t max_size = 2<<30;
			if (size > max_size) {
				throw make_range_exception("Requested reserve size {} exceeds maximum allowed {}", size, max_size);
			}
			list.size_to(len, capacity, size);
			capacity = size;
		}

	private:
		array_storage::array_storage<T> list;
		uint32_t len;
		uint32_t capacity;
		inline constexpr bool use_mem_ops() const {
			if constexpr (is_trivially_copyable) {
				constexpr size_t mem_oper_mul = 128;
				return len * sizeof(T) >= mem_oper_mul;
			}
			else {
				return false; // compile-time eliminated branch
			}
		}

	};
	template<typename T>
	inline void swap(array<T>& a, array<T>& b) noexcept {
		a.swap(b);
	}
	template<typename T>
	array(std::initializer_list<T>) -> array<T>;
	template<std::ranges::forward_range Range>
	array(Range&& r) -> array<std::ranges::range_value_t<Range>>;
}
#include <format>
#include <concepts>
namespace std {

	template<typename T>
	struct formatter<stn::array<T>>
	{
		constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

		template<typename FormatContext>
		auto format(const stn::array<T>& arr, FormatContext& ctx) const {
			// Compile-time check
			static_assert(
				std::is_arithmetic_v<T> || requires(T t, FormatContext & ctx2) {
					{ std::format_to(ctx2.out(), "{}", t) } -> std::same_as<typename FormatContext::iterator>;
			},
				"T must be arithmetic or have a valid std::formatter"
				);
			auto out = ctx.out();
			*out++ = '[';
			for (size_t i = 0; i < arr.length(); ++i) {
				if (i > 0) *out++ = ',';
				out = std::format_to(out, "{}", arr[i]);
			}
			*out++ = ']';
			return out;
		}
	};

}