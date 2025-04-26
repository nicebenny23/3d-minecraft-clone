#pragma once
#include <type_traits>
namespace Unique {
	template<typename T>
	struct UniquePtr {
	private:
		T* ptr;
		void Delete() {
			if constexpr (std::is_array<T>::value)
			{
				delete[] ptr;
			}
			else
			{
				delete ptr;
			}
			ptr = nullptr;
		}
	public:
		T& operator*() const {
			return *ptr;
		};
		T& operator->() const {
			return *ptr;
		}
		UniquePtr(UniquePtr&& other) {
			ptr = other.ptr;
			other.ptr = nullptr;
		}

		UniquePtr(const UniquePtr& other) = delete;
		void operator=(const UniquePtr& other) = delete;


		void operator=(UniquePtr&& other) noexcept {
			if (this == &other)
			{
				return;
			}

			Delete();
			ptr = other.ptr;
			other.ptr = nullptr;


		}
		~UniquePtr() {
			Delete();
		}
		T* Get() const {
			return ptr;
		}
		void Set(T* p = nullptr) {

			Delete();
			ptr = p;
		}
		explicit UniquePtr(T* Ptr = nullptr) noexcept :ptr(Ptr) {};

		template<typename U = T>
		//Template I found online
		typename std::enable_if<std::is_array<U>::value, T&>::type operator[](std::size_t index) const {
			return ptr[index];
		}
	};
}