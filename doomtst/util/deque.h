
#pragma once
#include <stdexcept>
#include "dynamicarray.h"
namespace Cont {
	template<typename T>
	struct deque {
		T* list;
		size_t capacity;
		size_t front;
		size_t back;
		size_t length;
		bool empty() const {
			return length == 0;
		}
		bool full() const
		{
			return length == capacity;
		}
		deque();



		T pop_front();
		T pop_back();
		void destroy();
		void resize();
		void push_back(const T& val);

		void push_front(const T& val);

		deque(deque&& other) noexcept;
		deque(const deque& other);
		deque& operator=(deque&& other) noexcept;
		deque& operator=(const deque& other);

		~deque() {
			destroy();
		}

		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = T;
		using difference_type = std::ptrdiff_t;
		using pointer = T*;
		using reference = T&;
		struct iterator {
			deque* arr;
			size_t index;

			iterator(deque* a, size_t idx) : arr(a), index(idx) {}

			bool operator==(const iterator& other) const {
				return arr == other.arr && index == other.index;
			}
			bool operator!=(const iterator& other) const {
				return !(*this == other);
			}

			T& operator*() const {
				return arr->list[index];
			}
			T* operator->() const {
				return &arr->list[index];
			}

			iterator& operator++() {
				arr->inc_index(index);
				return *this;
			}

			iterator operator++(int) {
				iterator temp = *this;
				++(*this);
				return temp;
			}
		};

		iterator begin() {
			return iterator(this, front);
		}
		iterator end() {
			size_t end_pos = back;
			inc_index(end_pos);
			return iterator(this, end_pos);
		}



	private:
		void inc_index(size_t& index) {
			index = (index == capacity - 1) ? 0 : index + 1;
		}

		void dec_index(size_t& index) {
			index = (index == 0) ? capacity - 1 : index - 1;
		}

	};
	template<typename T>
	void deque<T>::push_front(const T& val)
	{
		if (full())
			resize();

		if (!empty())
			dec_index(front);

		list[front] = val;
		length++;
	}

	template<typename T>
	void deque<T>::push_back(const T& val)
	{
		if (full())
			resize();

		if (!empty())
			inc_index(back);

		list[back] = val;
		length++;
	}

	template<typename T>
	void deque<T>::resize()
	{
		size_t new_size = resizelength(length);
		T* newlist = new T[new_size];

		if (list != nullptr)
		{
			size_t ind = front;
			for (size_t i = 0; i < length; i++) {
				newlist[i] = std::move(list[ind]);
				inc_index(ind);
			}
			delete[] list;
		}
		front = 0;
		back = length - 1;
		list = newlist;
		capacity = new_size;
	}




	template<typename T>
	T deque<T>::pop_front()
	{
		if (empty())
			throw std::out_of_range("Queue is empty");

		T val = list[front];
		inc_index(front);
		length--;
		return val;
	}

	template<typename T>
	T deque<T>::pop_back()
	{
		if (empty())
			throw std::out_of_range("Queue is empty");

		T val = list[back];
		dec_index(back);
		length--;
		return val;
	}

	template<typename T>
	void deque<T>::destroy() {
		delete[] list;
		list = nullptr;
		capacity = 0;
		length = 0;
		front = 0;
		back = 0;
	}
	template<typename T>
	inline deque<T>::deque()
	{
		list = nullptr;
		capacity = 0;
		front = 0;
		back = 0;
		length = 0;

	}
	template<typename T>
	inline deque<T>::deque(deque&& other) noexcept
	{
		list = other.list;
		capacity = other.capacity;
		length = other.length;
		front = other.front;
		back = other.back;
		other.list = nullptr;
		other.capacity = 0;
		other.length = 0;
		other.front = 0;
		other.back = 0;

	}

	template<typename T>
	inline deque<T>::deque(const deque& other)
	{
		list = new T[other.capacity];
		for (size_t i = 0; i < other.length; ++i) {
			size_t idx = (other.front + i) % other.capacity;
			list[i] = other.list[idx];
		}
		length = other.length;
		capacity = other.capacity;
		front = 0;
		back = length - 1;
	}

	template<typename T>
	deque<T>& deque<T>::operator=(deque<T>&& other) noexcept
	{
		if (this != &other) {
			destroy();
			list = other.list;
			capacity = other.capacity;
			length = other.length;
			front = other.front;
			back = other.back;

			// Reset the moved-from object
			other.list = nullptr;
			other.capacity = 0;
			other.length = 0;
			other.front = 0;
			other.back = 0;

		}
		return *this;
	}

	template<typename T>
	deque<T>& deque<T>::operator=(const deque<T>& other)
	{
		if (this != &other) {
			destroy();
			list = new T[other.capacity];
			for (size_t i = 0; i < other.length; ++i) {
				size_t idx = (other.front + i) % other.capacity;
				list[i] = other.list[idx];
			}
			length = other.length;
			capacity = other.capacity;
			front = 0;
			back = length - 1;
		}
		return *this;
	}








}