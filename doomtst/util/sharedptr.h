#pragma once
#include <stdexcept>
#include <concepts>
#include "Option.h"

namespace stn {

	inline void increment_counter(std::atomic<size_t>& c) {
		c.fetch_add(1, std::memory_order_relaxed);
	}

	inline void increment_counter(size_t& c) {
		++(c);
	}

	inline bool decrement_counter(std::atomic<size_t>& c) {
		return c.fetch_sub(1, std::memory_order_acq_rel) == 1;
	}

	inline bool decrement_counter(size_t& c) {
		return --(c) == 0;
	}

	inline size_t get_counter(const std::atomic<size_t>& c) {
		return c.load(std::memory_order_relaxed);
	}

	inline size_t get_counter(const size_t& c) {
		return c;
	}

	template<typename C>
	concept RefrenceCounter = stn::OneOf<C, size_t, std::atomic<size_t>>;

	template<typename T, RefrenceCounter C>
	struct rc_ptr_generic;

	template <typename T, RefrenceCounter C>
	struct rc_generic {

		template<typename ...Args>
			requires std::constructible_from<T, Args...>
		explicit rc_generic(Args&&... args)
			: count(new C(1)), ptr(new T(std::forward<Args>(args)...)) {
		}

		rc_generic(const rc_generic& other)
			: ptr(other.ptr), count(other.count) {
			increment_counter(*count);
		}

		rc_generic& operator=(const rc_generic& other) {
			if (this != &other) {
				increment_counter(*other.count);
				if (decrement_counter(*count)) {
					delete ptr;
					delete count;
				}

				ptr = other.ptr;
				count = other.count;
			}
			return *this;
		}

		template <typename U>
			requires std::has_virtual_destructor_v<U>
		rc_generic<U, C> upcast() requires std::derived_from<T, U> {
			increment_counter(*count);
			return rc_generic<U, C>(ptr, count);
		}

		template<typename U>
		rc_generic<U, C> downcast_unchecked() requires std::derived_from<U, T> {
			increment_counter(*count);
			return rc_generic<U, C>(static_cast<U*>(ptr), count);
		}

		T& operator*() {
			return *ptr;
		}
		const T& operator*() const {
			return *ptr;
		}

		T* operator->() {
			return ptr;
		}
		const T* operator->() const {
			return ptr;
		}

		std::size_t use_count() const {
			return get_counter(*count);
		}
		bool sole_owner() const {
			return use_count() == 1;
		}

		~rc_generic() {
			if (decrement_counter(*count)) {
				delete ptr;
				delete count;
			}
		}

		static stn::Option<rc_generic<T, C>> from_rc_ptr(const rc_ptr_generic<T, C>& rp);

	private:
		T* ptr;
		C* count;

		template<typename, RefrenceCounter>
		friend struct rc_ptr_generic;

		template<typename, RefrenceCounter>
		friend struct rc_generic;

		rc_generic(T* ptr, C* cnt)
			: ptr(ptr), count(cnt) {
		}
	};

	template <typename T, RefrenceCounter C>
	struct rc_ptr_generic {

		rc_ptr_generic()
			: count(nullptr), ptr(nullptr) {
		}

		template<typename ...Args>
			requires std::constructible_from<T, Args...>
		rc_ptr_generic(Args&&... args)
			: count(new C(1)), ptr(new T(std::forward<Args>(args)...)) {
		}

		std::size_t use_count() const noexcept {
			return count ? get_counter(*count) : 0;
		}

		explicit operator bool() const noexcept {
			return ptr != nullptr;
		}

		rc_ptr_generic(const rc_ptr_generic& other)
			: ptr(other.ptr), count(other.count) {
			inc();
		}

		rc_ptr_generic& operator=(const rc_ptr_generic& other) {
			if (this!=other) {
				dec();
				ptr = other.ptr;
				count = other.count;
				inc();
			}
			return *this;
		}

		rc_ptr_generic(rc_ptr_generic&& other) noexcept
			: ptr(other.ptr), count(other.count) {
			other.ptr = nullptr;
			other.count = nullptr;
		}

		rc_ptr_generic& operator=(rc_ptr_generic&& other) {
			if (this!=&other) {
				dec();
				ptr = other.ptr;
				count = other.count;
				other.ptr = nullptr;
				other.count = nullptr;
			}
			return *this;
		}

		template <typename U>
			requires std::has_virtual_destructor_v<U>
		rc_ptr_generic<U, C> upcast() requires std::derived_from<T, U> {
			inc();
			return rc_ptr_generic<U, C>(ptr, count);
		}

		template<typename U>
		rc_ptr_generic<U, C> downcast_unchecked() requires std::derived_from<U, T> {
			inc();
			return rc_ptr_generic<U, C>(static_cast<U*>(ptr), count);
		}

		T& operator*() {
			return *ptr;
		}
		const T& operator*() const {
			return *ptr;
		}

		T* operator->() {
			return ptr;
		}
		const T* operator->() const {
			return ptr;
		}

		void clear() {
			dec();
			ptr = nullptr;
			count = nullptr;
		}

		~rc_ptr_generic() {
			dec();
		}

		T* get_ptr() const noexcept {
			return ptr;
		}

		C* get_count_ptr() const noexcept {
			return count;
		}

		rc_ptr_generic(const rc_generic<T, C>& r)
			: count(r.count), ptr(r.ptr) {
			increment_counter(*r.count);
		}

	private:
		void inc() {
			if (count) {
				increment_counter(*count);
			}
		}

		void dec() {
			if (count && decrement_counter(*count)) {
				delete ptr;
				delete count;
			}
		}

		T* ptr;
		C* count;

		rc_ptr_generic(T* ptr, C* cnt)
			: ptr(ptr), count(cnt) {
		}

		template<typename, RefrenceCounter>
		friend struct rc_generic;

		template<typename, RefrenceCounter>
		friend struct rc_ptr_generic;
	};

	//returns an Option<rc<T>> which has a value if rc_ptr<T> has a value
	template<typename T, RefrenceCounter C>
	inline stn::Option<rc_generic<T, C>>
		rc_generic<T, C>::from_rc_ptr(const rc_ptr_generic<T, C>& rp) {
		if (!rp) {
			return stn::None;
		}

		increment_counter(*rp.count);
		return rc_generic<T, C>(rp.ptr, rp.count);
	}

	template<typename T>
	using rc = rc_generic<T, size_t>;

	template<typename T>
	using arc = rc_generic<T, std::atomic<size_t>>;

	template<typename T>
	using rc_ptr = rc_ptr_generic<T, size_t>;

	template<typename T>
	using arc_ptr = rc_ptr_generic<T, std::atomic<size_t>>;
}