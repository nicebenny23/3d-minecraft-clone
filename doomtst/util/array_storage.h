
#pragma once
#include <cstddef>
#include <type_traits>
#include <vector>
namespace array_storage {
	template <typename T>
	struct aligned_store {
		template<typename ...Args>
		void construct_at(size_t index, Args&&... args) requires(std::is_constructible_v<T,Args&&...>){
			new (get_ptr(index)) T(std::forward<Args>(args)...);
		}
		aligned_store() = default;

		void expand_at(size_t index, const T& value) {
			*(get_ptr(index)) = value;

		}
		void expand_at(size_t index, T&& value) {
			*(get_ptr(index)) = std::move(value);

		}
		void clear() {
			if (storage != nullptr)
			{
				::operator delete(storage);
				storage = nullptr;
			}
		}
		void destruct_at(size_t index) {
			get_ptr(index)->~T();
		}

		const T& operator[](size_t index) const {
			return storage[index];
		}
		//contract index<length
		T& operator[](size_t index) {
			return storage[index];
		}
		//never used
		void mem_copy_from(const aligned_store& other, size_t length) {
			//should never be called ;
			std::unreachable();
		}
		void size_to(size_t len, size_t capacity, size_t new_capacity) {
			// allocate new raw byte buffer
			T* new_buf = static_cast<T*>(::operator new(sizeof(T) * new_capacity));
			// move-construct existing elements into new buffer
			if (storage != nullptr) {
				for (size_t i = 0; i < len; ++i) {
					new (new_buf + i) T(std::move(storage[i]));
					storage[i].~T();
				}
				clear();
			}
			storage = new_buf;
		}

		explicit operator bool() const {
			return storage != nullptr;
		}
		const T* data() const {
			return storage;
		}
		T* data() {
			return storage;
		}

		void swap(aligned_store& other) {
			std::swap(storage, other.storage);
		}
		aligned_store(aligned_store&& other) {
			storage = other.storage;
			other.storage = nullptr;
		}
		aligned_store& operator=(aligned_store&& other) {
			storage = other.storage;
			other.storage = nullptr;
			return *this;
		}
	private:

		const T* get_ptr(size_t index) const {
			return storage + index;
		}
		T* get_ptr(size_t index) {
			return storage+ index;
		}


	private:
		T* storage = nullptr;

	};







	template <typename T>
	struct default_store {
		T& operator[](size_t index) {
			return storage[index];
		}
		const T& operator[](size_t index) const {
			return storage[index];
		}
		default_store() = default;
		void clear() {
			if (storage != nullptr)
			{
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
			get_ptr(index)->~T();
		}
		void mem_copy_from(const default_store& other, size_t length) {
			std::memcpy(storage, other.storage, sizeof(T) * length);
		}
		void size_to(size_t len, size_t capacity, size_t new_capacity) {

			T* newlist;
			if constexpr (std::is_trivially_copyable_v<T>)
			{
				if constexpr (std::is_trivially_default_constructible_v<T>) {
					if (capacity == 0)
					{

						newlist = (T*)std::calloc(new_capacity, sizeof(T));
					}
					else {
						newlist = (T*)realloc((void*)storage, sizeof(T) * new_capacity);
						std::memset(newlist + capacity, 0, (new_capacity - capacity) * sizeof(T));
						
					}

				}
				else {
					if (capacity == 0)
					{
						newlist = (T*)malloc(sizeof(T) * new_capacity);
					}
					else {
						newlist = (T*)realloc((void*)storage, sizeof(T) * new_capacity);
					}
				
				}

			}
			else
			{
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
		void swap(default_store& other) {
			std::swap(storage, other.storage);
		}
		default_store(default_store&& other) noexcept {
			storage = other.storage;
			other.storage = nullptr;
		}
		default_store& operator=(default_store&& other) noexcept {
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
	private:
		const T* get_ptr(size_t index) const {
			return storage + index;
		}
		T* get_ptr(size_t index) {
			return storage + index;
		}
		T* storage = nullptr;
	};

	template <typename T>
	using array_storage = std::conditional_t<std::is_default_constructible_v<T>, default_store<T>, aligned_store<T>>;

}