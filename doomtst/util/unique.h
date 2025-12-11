#pragma once
#include <type_traits>
#include <concepts>
namespace stn {
	template<typename T>
	struct box {

		template<typename... Args> requires std::constructible_from<T, Args&&...>
		explicit box(Args&&... args) : ptr(new T(std::forward<Args>(args)...)) {
		}
		box(box&& other) {
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		void operator=(const box& other) = delete;
		box(const box& other) = delete;
		template<typename U>
		box(box<U>&& other)	requires std::is_convertible_v<U*, T*>
			: ptr(other.ptr) {
			other.ptr = nullptr;
		}
		void operator=(box&& other) noexcept {
			if (this == &other) {
				return;
			}
			clear();
			ptr = other.ptr;
			other.ptr = nullptr;
		}

		explicit operator bool() const noexcept {
			return ptr;
		}
		T& operator*() {
			return *ptr;
		};
		const T& operator*() const {
			return *ptr;
		};
		T* operator->() {
			return ptr;
		}
		const T* operator->() const {
			return ptr;
		}
		template<typename U> requires std::derived_from<U, T>
		U& ref_as_unchecked() {
			return *static_cast<U*>(ptr);
		}
		template<typename U> requires std::derived_from<U, T>
		const U& ref_as_unchecked() const {
			return *static_cast<U*>(ptr);
		}
		template<typename U> requires std::derived_from<U, T>
		U* get_as_unchecked() {
			return static_cast<U*>(ptr);
		}
		template<typename U> requires std::derived_from<U, T>
		const U* get_as_unchecked() const {
			return static_cast<U*>(ptr);
		}
		T* get() {
			return ptr;
		}
		const T* get() const {
			return ptr;
		}
		T& operator[](std::size_t index) requires std::is_array_v<T> {
			return ptr[index];
		}
		const T& operator[](std::size_t index) const  requires std::is_array_v<T> {
			return ptr[index];
		}
		T* unbox() noexcept {
			T* temp = ptr;
			ptr = nullptr;
			return temp;
		}
		void clear() {
			if (ptr) {
				if constexpr (std::is_array_v<T>) {
					delete[] ptr;
				}
				else {
					delete ptr;
				}
				ptr = nullptr;
			}
		}

		~box() {
			clear();
		}
		void swap(box& other) noexcept {
			std::swap(ptr, other.ptr);
		}
	private:
		explicit box(T* pointer) noexcept : ptr(pointer) {
		}
		template<typename U>
		friend struct box;
		T* ptr;

	};
	template<typename T>
	void swap(box<T>& lhs, box<T>& rhs) noexcept {
		lhs.swap(rhs);
	}
}