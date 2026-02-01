
#pragma once
#include <cstddef>
#include <type_traits>
#include <vector>
namespace array_storage {




	template <typename T>
	struct array_storage {
		static constexpr bool is_aligned = !std::is_default_constructible_v<T>;
		T& operator[](size_t index) {
			return storage[index];
		}
		const T& operator[](size_t index) const {
			return storage[index];
		}
		array_storage() = default;
		void clear() {
			if (storage != nullptr) {
				free(storage);
				storage = nullptr;
			}
		}
		template<typename ...Args>
		void construct_at(size_t index, Args&&... args) {
			new (get_ptr(index)) T(std::forward<Args>(args)...);
		}
		void expand_at(size_t index, const T& value) {
			*(get_ptr(index)) = value;

		}
		void expand_at(size_t index, T&& value) {
			*(get_ptr(index)) = std::move(value);

		}
		void destruct_at(size_t index) {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				get_ptr(index)->~T();
			}
		}
		void copy_from(const array_storage& other, size_t length) {
			if constexpr (std::is_trivially_copyable_v<T>) {
				std::memcpy(storage, other.storage, sizeof(T) * length);
			}
			else {
				for (size_t i = 0; i < length; ++i) {
					new (get_ptr(i)) T(other[i]);
				}
			}
		}
		void size_to(size_t len, size_t capacity, size_t new_capacity) {
			T* newlist;

			if constexpr (std::is_trivially_copyable_v<T>) {
				if constexpr (std::is_trivially_default_constructible_v<T>) {
					if (capacity == 0) {

						newlist = (T*)std::calloc(new_capacity, sizeof(T));
					}
					else {
						newlist = (T*)realloc((void*)storage, sizeof(T) * new_capacity);
						std::memset(newlist + capacity, 0, (new_capacity - capacity) * sizeof(T));

					}

				}
				else {
					if (capacity == 0) {
						newlist = (T*)malloc(sizeof(T) * new_capacity);
					}
					else {
						newlist = (T*)realloc((void*)storage, sizeof(T) * new_capacity);
					}

				}

			}
			else {
				newlist = static_cast<T*>(malloc(sizeof(T) * new_capacity));

				for (size_t i = 0; i < len; ++i) {
					new (newlist + i) T(std::move(storage[i]));
					storage[i].~T();
				}
				clear();
			}
			storage = newlist;
		}
		explicit operator bool() const {
			return storage != nullptr;
		}
		void swap(array_storage& other) {
			std::swap(storage, other.storage);
		}
		array_storage(array_storage&& other) noexcept {
			storage = other.storage;
			other.storage = nullptr;
		}
		array_storage& operator=(array_storage&& other) noexcept {
			storage = other.storage;
			other.storage = nullptr;
			return *this;
		}
		const T* data() const {
			return storage;
		}
		T* data() {
			return storage;
		}

		const T* get_ptr(size_t index) const {
			return storage + index;
		}
		T* get_ptr(size_t index) {
			return storage + index;
		}
		T* storage = nullptr;
	};

}