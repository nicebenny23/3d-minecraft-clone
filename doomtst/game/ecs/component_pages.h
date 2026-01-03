#include <stdint.h>
#include <stdio.h>
#include <type_traits>
#include <concepts>
#include "../../util/dynamicarray.h"
#include "../../util/bitset.h"
#pragma once
namespace ecs {
	//Invariant: Destructors of all elements are called exactly once when removed.
	template<typename T, size_t chunk_exp>
	struct component_page {

		bool nonempty() const {
			return element_count != 0;
		}
		bool empty() const {
			return element_count == 0;
		}
		//index unchecked
		T& operator[](size_t index) {
			return *reinterpret_cast<T*>(element_offset_ptr + sizeof(T) * index);
		}
		const T& operator[](size_t index) const {
			return *reinterpret_cast<const T*>(element_offset_ptr + sizeof(T) * index);
		}
		T* get_ptr(size_t index) {
			return reinterpret_cast<T*>(element_offset_ptr + sizeof(T) * index);
		}
		const T* get_ptr(size_t index) const {
			return reinterpret_cast<const T*>(element_offset_ptr + sizeof(T) * index);
		}

		component_page(const component_page&) = delete;
		component_page& operator=(const component_page&) = delete;
		component_page() {
			elems = nullptr;
			element_offset_ptr = 0;
			element_count = 0;
		}
		// Move constructor
		component_page(component_page&& other) noexcept
			: elems(other.elems),
			element_offset_ptr(other.element_offset_ptr),
			element_count(other.element_count) {
			other.elems = nullptr;
			other.element_offset_ptr = 0;
			other.element_count = 0;
		}

		// Move assignment
		component_page& operator=(component_page&& other) noexcept {
			if (this != &other) {
				clear_unchecked();
				elems = other.elems;
				element_offset_ptr = other.element_offset_ptr;
				element_count = other.element_count;
				other.elems = nullptr;
				other.element_offset_ptr = 0;
				other.element_count = 0;
			}
			return *this;
		}

		size_t chunk_size() const {
			return 1 << chunk_exp;
		}

		//Assumes no object exists at the index
		template<typename ...Args>
		T& insert_at_unchecked(size_t index, Args&&... args) {
			if (empty()) {
				elems = static_cast<T*>(::operator new(chunk_size() * sizeof(T)));
				//we move element_offset back so we can save some cycles
				element_offset_ptr = reinterpret_cast<uintptr_t>(elems) - ((index >> chunk_exp) << chunk_exp) * sizeof(T);
			}
			T* ptr = get_ptr(index);
			::new (ptr) T(std::forward<Args>(args)...);
			element_count++;
			return *ptr;
		}


		//Assumes an object exists at the index 
		void remove_at_unchecked(size_t index) {
			get_ptr(index)->~T();
			--element_count;
			if (empty()) {
				clear_unchecked();
			}
		}

		//terminates if it is non empty, since the component page is an implmentation detail and component pages is safe this will never happen
		void clear_unchecked() {
			assert(empty() && "Index out of bounds in component_page");
			if (elems) {
				::operator delete(elems);
			}
			elems = nullptr;
			element_count = 0;
			element_offset_ptr = 0;
		}
		//terminates if it is non empty, since the component page is an implmentation detail and component pages is safe this will never happen
		~component_page() {
			clear_unchecked();
		}
	private:
		size_t element_count;
		T* elems;
		// element_offset_ptr is shifted so we can just add the index*sizeof(T) to it instaid of having to do a bitwise and
		uintptr_t element_offset_ptr;
	};

	template<typename T, size_t chunk_exp>
	struct component_pages {
		component_pages() :pages() {
		}

		component_pages(const component_pages& other) = delete;

		component_pages& operator=(const component_pages& other) = delete;

		component_pages(component_pages&& other) noexcept
			: pages(std::move(other.pages)),
			element_filter(std::move(other.element_filter)) {
			other.clear();
		}
		component_pages& operator=(component_pages&& other) noexcept {
			if (this != &other) {
				clear(); // delete current components
				pages = std::move(other.pages);
				element_filter = std::move(other.element_filter);
				other.clear();
			}
			return *this;
		}

		T& operator[](size_t index) {
			if (!contains(index)) {
				stn::throw_logic_error("element with page {} and index {} does not exist", index >> chunk_exp, index);
			}
			return pages.unchecked_at(index >> chunk_exp)[index];
		}

		const T& operator[](size_t index) const {
			if (!contains(index)) {
				stn::throw_logic_error("element with page {} and index {} does not exist", index >> chunk_exp, index);
			}
			return pages.unchecked_at(index >> chunk_exp)[index];
		}

		T& unchecked_at(size_t index) {
			return pages.unchecked_at(index >> chunk_exp)[index];
		}

		const T& unchecked_at(size_t index) const {
			return pages.unchecked_at(index >> chunk_exp)[index];
		}
		//does not check if this slot is also occupied
		template<typename ...Args>
		T& insert_at_unchecked(size_t index, Args&&... args) {
				element_filter.reaching_enable(index);
				return pages.reach(index >> chunk_exp).insert_at_unchecked(index, std::forward<Args>(args)...);
		}
		template<typename ...Args>
		T& insert_at(size_t index, Args&&... args) {
			if (!contains(index)) {
				element_filter.reaching_enable(index);
				return pages.reach(index >> chunk_exp).insert_at_unchecked(index, std::forward<Args>(args)...);
			}
			return pages.unchecked_at(index >> chunk_exp)[index];
		}

		//does not check if this slot is also occupied or exists
		void remove_at_unchecked(size_t index) {
				element_filter.disable_unchecked(index);
				pages.unchecked_at(index >> chunk_exp).remove_at_unchecked(index);
		}
		void remove_at(size_t index) {
			if (contains(index)) {
				element_filter.disable_unchecked(index);
				pages.unchecked_at(index >> chunk_exp).remove_at_unchecked(index);
			}

		}

		bool contains(size_t index) const {
			return element_filter.contains(index);
		}

		size_t chunk_size() const {
			return 1 << chunk_exp;
		}

		void clear() {
			stn::array<size_t> indices = element_filter.indices<size_t>();
			for (size_t index : indices) {
				remove_at(index);
			}
			element_filter.clear();
			pages.clear();
		}

		~component_pages() {
			clear();
		}
	private:
		stn::array<component_page<T, chunk_exp>> pages;
		stn::bitset element_filter;
	};
}