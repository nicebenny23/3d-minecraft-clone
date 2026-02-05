#pragma once
#include <type_traits>
#include <concepts>
#include <vector>
#include "concepts.h"
#include "exception.h"
namespace stn {

	//never use
	struct construct_null_tag {
	};
	inline constexpr construct_null_tag construct_null= construct_null_tag();
	template<typename D>
	struct construct_derived {
		using type = D;
		constexpr construct_derived() = default;
	};

	template<typename Tag>
	concept ConstructDerivedTagType =
		requires { typename Tag::type; }&&
	std::is_same_v<Tag, construct_derived<typename Tag::type>>;
	template<ConstructDerivedTagType Tag>
	using ConstructDerivedType = typename Tag::type;
	template<typename T>
	struct box {
		
		explicit box(T&& value) : ptr(new T(std::move(value))) {
		}
		template<ConstructDerivedTagType Tag>
		requires std::derived_from<typename Tag::type, T>
		&& stn::CompleteVirtual<T>
			explicit box(Tag, Tag::Type&& U)
			: ptr(new Tag::Type(std::move(U))) {
		}
		using value_type = T;
		template<typename... Args>
		requires( (!std::same_as<box<T>,Args>&&...)
		&& stn::RecursiveConstructible<T, Args&&...>)
		explicit box(Args&&... args) : ptr(new T(std::forward<Args>(args)...)) {
		}
		
		template<ConstructDerivedTagType Tag, typename... Args>
			requires std::derived_from<typename Tag::type, T>&&
		stn::RecursiveConstructible<typename Tag::type, Args...>
			&& stn::CompleteVirtual<T>
			explicit box(Tag&&, Args&&... args)
			: ptr(new typename Tag::type(std::forward<Args>(args)...)) {
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
		template<ConstructDerivedTagType Tag>
			requires std::derived_from<ConstructDerivedType<Tag>, T>&&
		std::copy_constructible<ConstructDerivedType<Tag>>
		box<T> clone_as() const
		{
			using U = ConstructDerivedType<Tag>;
			const U* ptr_as_u= dynamic_cast<U>(ptr);   
			if (ptr) {
				return box<T>(new U(*ptr_as_u));
			}
			stn::throw_logic_error("ptr was not of U");
		}
		box<T> clone() const requires std::copy_constructible<T> {
			return box<T>(new T(ptr));
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

		~box() noexcept{
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