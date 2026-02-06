#pragma once
#include <stdexcept>
#include <concepts>
#include "Option.h"
namespace stn {
	template<typename T>
	struct rc_ptr;

	template <typename T>
	struct rc {


		template<typename ...Args>
			requires std::constructible_from<T, Args...>
		rc(Args&&... args)
			: count(new unsigned int(1)), ptr(new T(std::forward<Args>(args)...)) {
		}

		rc(const rc& other) : ptr(other.ptr), count(other.count) {
			(*count)++;
		}
		rc& operator=(const rc<T>& other) {
			(*other.count)++;
			if (!--(*count)) {
				delete ptr;
				delete count;
			}

			ptr = other.ptr;
			count = other.count;
			return *this;

		}
		template <typename U> requires std::has_virtual_destructor_v<U>
		rc<U> upcast() requires std::derived_from<T, U> {
			(*count)++;
			return rc<U>(ptr, count);

		}
		template<typename U>
		rc<U> downcast_unchecked() requires std::derived_from<U, T> {
			(*count)++;          // increment reference count
			return rc<U>(static_cast<U*>(ptr), count);
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

		std::size_t use_count() const noexcept {
			return *count;
		}

		~rc() {
			if (!--(*count)) {
				delete ptr;
				delete count;
			}
		}
		static stn::Option<rc<T>> from_rc_ptr(const rc_ptr<T>& rp);
	private:
		T* ptr;
		size_t* count;
		template<typename U>
		friend struct rc_ptr;
		rc(T* ptr, size_t* cnt) :ptr(ptr), count(cnt) {

		}
	
		
	};
	template <typename T>
	struct rc_ptr {

		rc_ptr()
			: count(nullptr), ptr(nullptr) {
		}

		template<typename ...Args>
			requires std::constructible_from<T, Args...>
		rc_ptr(Args&&... args)
			: count(new size_t(1)), ptr(new T(std::forward<Args>(args)...)) {
		}

		std::size_t use_count() const noexcept {
			return count ? *count : 0;
		}

		explicit operator bool() const noexcept {
			return ptr != nullptr;
		}
		rc_ptr(const rc_ptr& other) : ptr(other.ptr), count(other.count) {
			inc();
		}
		rc_ptr& operator=(const rc_ptr<T>& other) {
			if (ptr != other.ptr) {
				dec();
				ptr = other.ptr;
				count = other.count;
				inc();
			}
			return *this;

		}
		rc_ptr(rc_ptr<T>&& other) noexcept
			: ptr(other.ptr), count(other.count) {
			other.ptr = nullptr;
			other.count = nullptr;
		}
		rc_ptr& operator=(rc_ptr<T>&& other) {
			if (ptr != other.ptr) {
				dec();
				ptr = other.ptr;
				count = other.count;
				other.ptr = nullptr;
				other.count = nullptr;
			}
			return *this;
		}

		template <typename U> requires std::has_virtual_destructor_v<U>
		rc_ptr<U> upcast() requires std::derived_from<T, U> {
			inc();
			return rc_ptr<U>(ptr, count);

		}
		template<typename U>
		rc_ptr<U> downcast_unchecked() requires std::derived_from<U, T> {
			inc();
			return rc_ptr<U>(static_cast<U*>(ptr), count);
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
		~rc_ptr() {
			dec();
		}

		T* get_ptr() const noexcept {
			return ptr;
		}
		size_t* get_count_ptr() const noexcept {
			return count;
		}
		rc_ptr(const rc<T>& r):count(r.count),ptr(r.ptr) {
			++(*r.count);
		}
	private:
		void inc() {
			if (count) {
				(*count)++;
			}
		}
		void dec() {
			if (count && !--(*count)) {
				delete ptr;
				delete count;
			}
		}
		T* ptr;
		size_t* count;
		rc_ptr(T* ptr, size_t* cnt) :ptr(ptr), count(cnt) {

		}
		template<typename U>
		friend struct rc;
	};

	template<typename T>
	inline stn::Option<rc<T>> rc<T>::from_rc_ptr(const rc_ptr<T>& rp) {
		if (!rp) {
			return stn::None;
		}
		(*rp.count)++;
		return rc<T>(rp.ptr, rp.count);
	}

}
