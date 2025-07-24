#pragma once
#include <type_traits>
namespace Unique {
	template<typename T>
	struct Box {
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
		Box(Box&& other) {
			ptr = other.ptr;
			other.ptr = nullptr;
		}

		Box(const Box& other) = delete;
		void operator=(const Box& other) = delete;


		void operator=(Box&& other) noexcept {
			if (this == &other)
			{
				return;
			}

			Delete();
			ptr = other.ptr;
			other.ptr = nullptr;


		}
		~Box() {
			Delete();
		}
		T* Get() const {
			return ptr;
		}
		void Set(T* p = nullptr) {

			Delete();
			ptr = p;
		}
		explicit Box(T* Ptr = nullptr) noexcept :ptr(Ptr) {};

		template<typename U = T>
		//Template I found online
		typename std::enable_if<std::is_array<U>::value, T&>::type operator[](std::size_t index) const {
			return ptr[index];
		}
	};
}