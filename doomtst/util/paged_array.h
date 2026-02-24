#include "dynamicarray.h"
#pragma once
namespace stn {
	static constexpr size_t page_size = 8;


	static constexpr size_t page_elements = 1 << page_size;

	static constexpr size_t page_elements_minus_one = page_elements - 1;
	template<typename T>
	struct paged_array {

	private:
		stn::array<T*> page_pointers;
		std::size_t len;
	public:
		//can be a bit faster with the offset_ptr aproach

		inline bool empty() const {
			return len == 0;
		}
		inline size_t size() const {
			return len;
		}
		inline size_t size_mod_page_elements() const {
			return len & page_elements_minus_one;
		}
		inline size_t length() const {
			return len;
		}
		template<typename ...Args> requires std::constructible_from<T, Args&&...>
		T& emplace(Args&&... args) {
			//
			size_t index = (len) & page_elements_minus_one;
			if (index == 0) {
				T* new_ptr = static_cast<T*>(::operator new(sizeof(T)* page_elements));
				page_pointers.push(new_ptr);
			}
			T* ptr_to = page_pointers.last_unchecked() + index;
			new (ptr_to) T(std::forward<Args>(args)...);
			len++;
			return *ptr_to;
		}
		void expand(size_t size) {
			if (len < size) {
				size_t needed = size - len;
				for (size_t i = 0; i < needed; i++) {
					emplace();
				}

			}
		}
		template<typename U> requires std::constructible_from<T, U&&>
		T& push(U&& value) {
			return emplace(std::forward<U>(value));
		}
		T& reach(size_t index) requires std::is_default_constructible_v<T> {
			if (length() <= index) {
				expand(index + 1);
			}
			return page_pointers.unchecked_at(index >> page_size)[index & page_elements_minus_one];
		}
		void clear() {
			size_t remainder = len;
			
			for (size_t i = 0; i < page_pointers.length(); i++) {
				T* last_ptr = page_pointers[i];
				size_t destroy_count =stn::min(remainder,page_elements);
				for (size_t i = 0; i < destroy_count; i++) {
					(*(last_ptr + i)).~T();
				}
				::operator delete(last_ptr);
				remainder -= destroy_count;
			}
			len = 0;
			page_pointers.clear();
		}
		paged_array() :len(0), page_pointers(0) {

		}
		paged_array(paged_array&& other) :page_pointers(std::move(other.page_pointers)), len(other.len) {
			other.len = 0;
		}
		paged_array& operator =(paged_array&& other) {
			if (this != &other) {

				clear();
				page_pointers = std::move(other.page_pointers);
				len = other.len;
				other.len = 0;
			}
			return *this;
		}
		~paged_array() {
			clear();
		}
		auto operator[](this auto&& self, size_t index)->stn::apply_const_like_t<decltype(self), T>& {
			if (self.length() <= index) {
				stn::throw_logic_error("cannot acess index {} in paged array of length {} ", index, self.length());
			}
			return self.page_pointers.unchecked_at(index >> page_size)[index & page_elements_minus_one];
		}
		auto unchecked_at(this auto&& self, size_t index) -> stn::apply_const_like_t<decltype(self), T>& {
			return self.page_pointers.unchecked_at(index >> page_size)[index & page_elements_minus_one];
		}

		template<typename U>
		struct iterator_template {
			size_t page_jump_counter;
			U* current_ptr;
			paged_array& array;
			U operator*() {
				return (*current_ptr)[page_jump_counter];
			}
			iterator_template& operator++() {
				if (++page_jump_counter == page_elements) {
					page_jump_counter = 0;
				}
				return *this;
			}



		};




	};
}


