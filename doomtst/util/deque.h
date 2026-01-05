#pragma once
#include "../debugger/debug.h"
#include <stdexcept>
#include "dynamicarray.h"
namespace stn {
	template<typename T>
	struct deque {
		constexpr bool empty() const {
			return len == 0;
		}
		bool full() const
		{
			return len == capacity;
		}
		size_t length() const{
			return len;
		}
	
		deque()
		{
			list = nullptr;
			capacity = 0;
			front = 0;
			back = 0;
			len = 0;
		}
		deque(deque&& other) noexcept
		{
			list = other.list;
			capacity = other.capacity;
			len = other.len;
			front = other.front;
			back = other.back;
			other.list = nullptr;
			other.capacity = 0;
			other.len = 0;
			other.front = 0;
			other.back = 0;

		}
		deque& operator=(deque&& other) noexcept
		{
			if (this != &other) {
				clear();
				list = other.list;
				capacity = other.capacity;
				len = other.len;
				front = other.front;
				back = other.back;
				other.list = nullptr;
				other.capacity = 0;
				other.len = 0;
				other.front = 0;
				other.back = 0;

			}
			return *this;
		}
		deque(const deque& other) requires std::is_copy_constructible_v<T> :deque()
		{
			reserve(other.capacity);
			for (size_t i = 0; i < other.len; ++i) {
				size_t idx = (other.front + i) % other.capacity;
				construct_at(i,other.list[idx]);
			}
			len = other.len;
			capacity = other.capacity;
			front = 0;
			back = (len == 0) ? 0 : len - 1;
		}
		deque& operator=(const deque& other) requires std::is_copy_constructible_v<T>
		{
			if (this != &other) {
				clear();
				reserve(other.capacity);
				for (size_t i = 0; i < other.len; ++i) {
					size_t idx = (other.front + i) % other.capacity;
					construct_at(i, other.list[idx]);
				}
				len = other.len;
				capacity = other.capacity;
				front = 0;
				back = (len == 0) ? 0 : len - 1;
			}
			return *this;
		}
		template<typename ...Args> requires std::constructible_from<T, Args&&...>
		void emplace_front(Args&&... args)
		{
			if (full()) {
				reserve(resize_length(capacity));
			}
			if (!empty()) {
				dec_index(front);
			}
			construct_at(front, std::forward<Args>(args)...);
			len++;

		}
		template<typename ...Args> requires std::constructible_from<T, Args&&...>
		void emplace_back(Args&&... args)
		{
			if (full()) {
				reserve(resize_length(capacity));
			}
			if (!empty()) {
				inc_index(back);
			}
			construct_at(back, std::forward<Args>(args)...);
			len++;

		}
		template<typename U> requires std::constructible_from<T, U&&>
		void push_back(U&& val)
		{
			emplace_back(std::forward<U>(val));
		}
		template<std::ranges::forward_range R>
			requires std::same_as<std::ranges::range_value_t<R>, T>
		void push_back(R&& range) {
			for (auto&& element : range) {
				push_back(std::forward<decltype(element)>(element));
			}
		}


		template<typename U> requires std::constructible_from<T, U&&>
		void push_front(U&& val)
		{
			emplace_front(std::forward<U>(val));
		}
		
		template<std::ranges::forward_range R>
			requires std::same_as<std::ranges::range_value_t<R>, T>
		void push_front(R&& range) {
			for (auto&& element : range) {
				push_front(std::forward<decltype(element)>(element));
			}
		}

		void drop_front()
		{
			if (empty())
			{
				stn::throw_out_of_range("cannot drop_front from an empty deque");
			}
			destruct_at(front);
			len--;
			if (!empty())
			{
				inc_index(front);
			}
		}

		void drop_back()
		{
			if (empty())
			{
				stn::throw_out_of_range("cannot drop_back from an empty deque");
			}
			destruct_at(back);
			len--;
			if (!empty())
			{
				dec_index(back);
			}
		}

		T pop_front()
		{
			if (empty())
			{
				stn::throw_out_of_range("cannot pop_front from an empty deque");
			}
			T val = std::move(list[front]);
			drop_front();
			return val;
		}

		T pop_back()
		{
			if (empty())
			{
				stn::throw_out_of_range("cannot pop_back from an empty deque");
			}
			T val = std::move(list[back]);
			drop_back();
			return val;
		}
		void drop_front_many(size_t count)
		{
			if (len<count)
			{
				stn::throw_out_of_range("drop_front_many: cannot drop {} elements when the deque only has {} ",count,len);
			}
			for (size_t i = 0; i < count; ++i) {
				destruct_at(front);
				inc_index(front);
			}
			len -= count;
			//resets if empty
			if (len == 0) {
				front = back = 0; 
			}
		}

		void drop_back_many(size_t count)
		{
			if (len < count)
			{
				stn::throw_out_of_range("drop_back_many: cannot drop {} elements when the deque only has {} ", count, len);
			}	
			for (size_t i = 0; i < count; ++i) {
				destruct_at(back);
				dec_index(back);
			}
			len -= count;
			//resets if empty
			if (len == 0) {
				front = back = 0;
			}
		}
		void clear() {
			if (list==nullptr)
			{
				return;
			}
			size_t ind = front;
			for (size_t i = 0; i < len; i++) {
				destruct_at(ind);
				inc_index(ind);
			}
			::operator delete(list);
			list = nullptr;
			capacity = 0;
			len = 0;
			front = 0;
			back = 0;
		}

		~deque() {
			clear();
		}

		template<typename U>
		struct iterator_base {
			using deque_store_type = std::conditional_t<std::is_const_v<U>, const deque*, deque*>;
			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = U;
			using difference_type = std::ptrdiff_t;
			using pointer = U*;
			using reference = U&;
			iterator_base(deque_store_type pages, size_t idx) : deque_store(pages), logical_pos(idx) {}
			iterator_base() :deque_store(nullptr), logical_pos(nullptr) {
			}
			bool operator==(const iterator_base& other) const {
				return deque_store == other.deque_store && logical_pos == other.logical_pos;
			}
			bool operator!=(const iterator_base& other) const {
				return !(*this == other);
			}
			reference operator*() const {
				size_t physical_index = (deque_store->front + logical_pos) % deque_store->capacity;
				return deque_store->list[physical_index];
			}
			pointer operator->() const {
				size_t physical_index = (deque_store->front + logical_pos) % deque_store->capacity;
				return &deque_store->list[physical_index];
			}
			iterator_base& operator++() {
				++logical_pos;
				return *this;
			}

			iterator_base operator++(int) {
				iterator_base temp = *this;
				++(*this);
				return temp;
			}
			iterator_base& operator--() {
				--logical_pos;
				return *this;
			}

			iterator_base operator--(int) {
				iterator_base temp = *this;
				--(*this);
				return temp;
			}
			iterator_base& operator+=(difference_type n) {
				logical_pos += n;
				return *this;
			}

			iterator_base operator+(difference_type n) const {
				iterator_base tmp = *this;
				tmp += n;
				return tmp;
			}

			friend iterator_base operator+(difference_type n, const iterator_base& it) {
				return it + n;
			}

			iterator_base& operator-=(difference_type n) {
				logical_pos -= n;
				return *this;
			}

			iterator_base operator-(difference_type n) const {
				iterator_base tmp = *this;
				tmp -= n;
				return tmp;
			}
		private:
			deque_store_type deque_store;
			size_t logical_pos;
		};
		using iterator = iterator_base<T>;
		iterator begin() { return iterator(this, 0); }
		iterator end() { return iterator(this, len); }

		using const_iterator = iterator_base<const T>;
		const_iterator begin() const { return const_iterator(this, 0); }
		const_iterator end()   const { return const_iterator(this, len); }


		void reserve(size_t new_capacity)
		{
			if (capacity < new_capacity)
			{
				T* old_list = list;
				list= (T*)(operator new(sizeof(T) * new_capacity));

				if (old_list!= nullptr)
				{
					size_t ind = front;
					for (size_t i = 0; i < len; i++) {
						construct_at(i, std::move(old_list[ind]));
						old_list[ind].~T();
						inc_index(ind);
					}
					::operator delete(old_list);
				}
				front = 0;
				back = (len == 0) ? 0 : len - 1;
				capacity = new_capacity;
			}
		}
	private:
		void inc_index(size_t& index) const {
			index = (index == capacity - 1) ? 0 : index + 1;
		}
		void dec_index(size_t& index) const {
			index = (index == 0) ? capacity - 1 : index - 1;
		}
		template<typename ...Args>
		void construct_at(size_t index, Args&&... args) {
			new (list + index) T(std::forward<Args>(args)...);
		}
		void destruct_at(size_t index) {
			(list + index)->~T();
		}
		T* list;
		size_t capacity;
		size_t front;
		size_t back;
		size_t len;

	};


}

