#pragma once
#include <type_traits>
#include <concepts>
namespace stn {


	template<typename D>
	struct construct_derived {
		using type = D;
		constexpr construct_derived() = default;
	};
	template<typename Tag>
	concept ConstructDerivedTagType =
		requires { typename Tag::type; }&&
	std::is_same_v<Tag, construct_derived<typename Tag::type>>;
	template<typename T>
	struct box {

		using element_type = T;
		template<typename... Args>
			requires std::constructible_from<T, Args&&...>
		explicit box(Args&&... args) : ptr(new T(std::forward<Args>(args)...)) {
		}

		template<ConstructDerivedTagType Tag, typename... Args>
			requires std::derived_from<typename Tag::type, T>&&
		std::constructible_from<typename Tag::type, Args&&...>
			explicit box(Tag, Args&&... args)
			: ptr(new typename Tag::type(std::forward<Args>(args)...)) {
		}



		box() requires std::default_initializable<T> : ptr(new T()) {
		}
		box(box&& other) noexcept {
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		void operator=(const box& other) = delete;
		box(const box& other) = delete;


		template<typename U>
			requires (std::derived_from<T, U>&& std::has_virtual_destructor_v<U>)
		box<U> upcast() && noexcept {
			box<U> other(static_cast<U*>(ptr));
			ptr = nullptr;
			return other;
		}

		template<typename U> requires std::derived_from<U, T>
		box<U> downcast_unchecked() && noexcept {
			box<U> other(static_cast<U*>(ptr));
			if (other) {
				ptr = nullptr;
			}
			return other;
		}

		template<typename U> requires std::derived_from<U, T>
		box<U> dynamic_downcast() && noexcept {
			box<U> other(dynamic_cast<U*>(ptr));
			if (other) {
				ptr = nullptr;
			}
			return other;
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
			return *static_cast<const U*>(ptr);
		}

		//requires the cast to be valid
		template<typename U> requires std::derived_from<U, T>
		U* get_as_unchecked() {
			return static_cast<U*>(ptr);
		}
		//requires the cast to be valid
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
		template<typename T>
		bool is() const {
			return dynamic_cast<const T*>(ptr) != nullptr;
		}
	private:
		explicit box(T* pointer) noexcept : ptr(pointer) {
		}
		template<typename U>
		friend struct box;
		T* ptr;

	};
	template<typename T>
	struct is_box : std::false_type {
	};

	template<typename T>
	struct is_box<box<T>> : std::true_type {
	};
	template<typename T>
	concept BoxType = is_box<T>::value;
}

namespace std {
	template<typename T>
	void swap(stn::box<T>& lhs, stn::box<T>& rhs) noexcept {
		lhs.swap(rhs);
	}
}