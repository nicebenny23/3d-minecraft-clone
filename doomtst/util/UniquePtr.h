#pragma once
#include <type_traits>
namespace Unique {
	template<typename T>
	struct boxed {
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
		boxed(boxed&& other) {
			ptr = other.ptr;
			other.ptr = nullptr;
		}

		boxed(const boxed& other) = delete;
		void operator=(const boxed& other) = delete;


		void operator=(boxed&& other) noexcept {
			if (this == &other)
			{
				return;
			}

			Delete();
			ptr = other.ptr;
			other.ptr = nullptr;


		}
		~boxed() {
			Delete();
		}
		T* Get() const {
			return ptr;
		}
		void Set(T* p = nullptr) {

			Delete();
			ptr = p;
		}
		explicit boxed(T* Ptr = nullptr) noexcept :ptr(Ptr) {};

		template<typename U = T>
		//Template I found online
		typename std::enable_if<std::is_array<U>::value, T&>::type operator[](std::size_t index) const {
			return ptr[index];
		}
	};
}