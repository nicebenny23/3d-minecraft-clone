#pragma once

#include <stdexcept>
#include <vector>
#include <new>
#include <cstdlib>
#include <cstring>
#include <initializer_list>

namespace stn {


	// Resizing function that doubles the size of the array when needed.
	// If the length is 0, it initializes to a default size of 2.
	inline size_t resizelength(size_t length) {
		return (length == 0) ? 2 : 2 * length;  // Double the capacity for efficient reallocation
	}

	template<class T, bool initelems = true>
	class array {
	public:




		bool empty();
		// Standard operator[] that returns a reference to the element at the given index.
		T& operator[](size_t index);
		T& reach(const size_t index);
		const T& operator[](size_t index) const;
		// At function that checks if the index is in range before returning the element.
		T& at(size_t index);

		// Cut a range of elements from startindex to endindex.
		void cutind(size_t startindex, size_t endindex);

		// Slice the array between a start and end index.
		void slice(size_t startindex, size_t endindex);

		// Insert an element at a specific index, shifting others.
		void insertind(size_t index, T value);

		// Delete an element at the given index, shifting others.
		void deleteind(size_t index);



		// Append a single value to the array.
	
		// Append an array of elements to the current array.
		void push(T* arr, size_t otherlen);

		// Append another array to this one.
		void push(array arr);


		T pop();

		// Destroy the array by deallocating memory and resetting values.
		void destroy();


		// Swap contents of two arrays efficiently.
		void swap(array& other);

		// Default constructor.
		constexpr array() noexcept;

		// Constructor that initializes the array with a given length
		array(size_t size);
		array(size_t len, const T& value);
		// Constructor that initializes the array from a raw posize_ter and size.
		array(T* arr, size_t size);
		array(std::initializer_list<T> init);
		// Move constructor.
		array(array&& other) noexcept;
		

		template<class InputIt>
		array(InputIt first, InputIt last);
		template<typename... Args>
		void push(Args&&... values);

		// Copy constructor.
		explicit array(const array& arr);

		// Move assignment operator.
		array& operator=(array&& other) noexcept;
		array& operator=(const array& other);
		bool operator==(const array& other) const;
		bool valid(size_t index) {

			return index < length;
		}
		// Posize_ter to the dynamically allocated array.
		T* list;
		// Current number of elements in the array.
		uint32_t length;

		// Total allocated capacity of the array.
		uint32_t capacity;
		T* data() {
			return list;
		}
		// Iterator class for accessing elements using range-based for loops.
		class Iterator {
		public:
			using iterator_category = std::random_access_iterator_tag;  // or forward_iterator_tag
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

		private:
			T* ptr;

		public:
			Iterator(T* p) : ptr(p) {}
			Iterator() : ptr(nullptr) {}
			reference operator*() const { return *ptr; }
			pointer operator->() const { return ptr; }

			Iterator& operator++() { ++ptr; return *this; }
			Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }

			Iterator& operator--() { --ptr; return *this; }
			Iterator operator--(int) { Iterator tmp = *this; --(*this); return tmp; }

			bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
			bool operator==(const Iterator& other) const { return ptr == other.ptr; }

			Iterator operator+(difference_type n) const { return Iterator(ptr + n); }
			Iterator operator-(difference_type n) const { return Iterator(ptr - n); }
			difference_type operator-(const Iterator& other) const { return ptr - other.ptr; }

			Iterator& operator+=(difference_type n) { ptr += n; return *this; }
			Iterator& operator-=(difference_type n) { ptr -= n; return *this; }

			reference operator[](difference_type n) const { return ptr[n]; }

			bool operator<(const Iterator& other) const { return ptr < other.ptr; }
			bool operator>(const Iterator& other) const { return ptr > other.ptr; }
			bool operator<=(const Iterator& other) const { return ptr <= other.ptr; }
			bool operator>=(const Iterator& other) const { return ptr >= other.ptr; }
		};
		using iterator = Iterator;

		class ConstIterator {
		private:
			const T* ptr;

		public:
			using value_type = T;
			using iterator = ConstIterator;
			using size_type = std::uint32_t;

			ConstIterator(const T* p) : ptr(p) {}

			const T& operator*() const {
				return *ptr;
			}

			ConstIterator& operator++() {
				++ptr;
				return *this;
			}
			ConstIterator& operator--() {
				--ptr;
				return *this;
			}

			bool operator!=(const ConstIterator& other) const {
				return ptr != other.ptr;
			}

			bool operator==(const ConstIterator& other) const {
				return ptr == other.ptr;
			}
		};

		T& last();
		T& first();
		// Methods to get the beginning and end iterators for range-based for loops.
		Iterator begin() {
			return Iterator(list);  // Return an iterator to the first element.
		}

		Iterator end() {
			return Iterator(list + length);  // Return an iterator past the last element.
		}
		using const_iterator = ConstIterator;
		const_iterator begin() const { return const_iterator(list); }
		const_iterator end()   const { return const_iterator(list + length); }
		const_iterator cbegin() const { return const_iterator(list); }
		const_iterator cend()   const { return const_iterator(list + length); }
		// Method to return the current capacity of the array.
		size_t get_size() const {
			return capacity;
		}

		//resize that changes length
		void expand(size_t size);
		void resize(size_t size = 0);
		static constexpr size_t npos = (uint32_t)-1;
		// Find the index of value, or npos if not found
		size_t find(const T& value) const;

		// Return true if value is in the array
		bool contains(const T& value) const; 

	};






	//deletes content of list(not posize_ters!!)

	template<class T, bool initelems>
	inline bool array<T, initelems>::empty()
	{
		return (length == 0);
	}

	//returns alias to index
	template<class T, bool initelems >
	inline T& array<T, initelems>::operator[](size_t index) {

	
		if (index >= length) {
			throw("[] cannot acess out of bounds; use reach");
		}
		return list[index];


	}
	template<class T, bool initelems>
	inline T& array<T, initelems>::reach(const size_t index)
	{

		if (index >= length)
		{
			length = index + 1;
		}
		if (index >= capacity)
		{
			resize(resizelength(index));

		}
		return list[index];
	}
	template<class T, bool initelems>
	inline const T& array<T, initelems>::operator[](size_t index) const
	{
		if (index >= length) {
			throw std::invalid_argument("Index out of bounds for const []");
		}

		return list[index];
	}
	//same as [] but only in bounds
	template<class T, bool initelems >
	T& array<T, initelems>::at(size_t index) {
		if (index >= length) {
			throw std::invalid_argument("Index out of bounds for insertion");
		}

		return list[index];


	}
	
	//keeps a range of indices including [start,end] ind,todo add inclusive exculsive toggle
	template<class T, bool initelems>
	void array<T, initelems>::slice(size_t startindex, size_t endindex) {


		if (endindex < startindex || endindex >= length)
		{

			throw	std::invalid_argument("Index out of bounds for slice");
		}
		//moves indices 
		for (size_t i = startindex; i <= endindex; i++)
		{
			list[i - startindex] = list[i];
		}

		// setting length to area remaining
		length = endindex - startindex + 1;

		return;

	}
	//removes a range of indexes [start,end]
	template<class T, bool initelems>
	void array<T, initelems>::cutind(size_t startindex, size_t endindex) {

		if (endindex < startindex || endindex >= length)
		{
			throw	std::invalid_argument("Index out of bounds for cutting");
		}

		size_t dif = endindex - startindex;

		for (size_t i = endindex + 1; i < length; i++)
		{
			list[i - dif - 1] = list[i];
		}


		length -= dif + 1;

	}

	//deleted the element at index
	template<class T, bool initelems>
	void array<T, initelems>::deleteind(size_t index) {

		if (index >= length)
		{
			throw std::invalid_argument("Index out of bounds for deletion");
		}



		length--;//decrements size
		for (size_t i = index; i < length; i++)
		{

			list[i] = list[i + 1];//this lets index be removed
		}


	}








	template<class T, bool initelems>
	void array<T, initelems>::insertind(size_t index, T value) {
		if (index > length)
		{
			throw std::invalid_argument("Index out of bounds for insertion");

		}
		if (length >= capacity)
		{
			resize(resizelength(length));
		}



		for (size_t i = length; i > index; i--)
		{
			list[i] = list[i - 1];
		}

		list[index] = value;

		length++;
	}

	//copies and appends an element to the list(!!!issues with posize_ters!!!)
	
	//append
	template<class T, bool initelems >
	inline void array<T, initelems>::push(T* arr, size_t otherlen)
	{

		if (length + otherlen >= capacity)
		{
			resize(resizelength((length + otherlen)));
		}


		for (size_t i = 0; i < otherlen; i++)
		{
			list[i + length] = arr[i];
		}
		length += otherlen;
	}
	//resizes the array
	//appends a list to the end of the list(doesent delete it)(!!!caution with posize_ter lists!!!)
	template<class T, bool initelems >
	void array<T, initelems>::push(array arr) {

		push(arr.list, arr.length);
	}

	template<class T, bool initelems>
	inline T array<T, initelems>::pop()
	{
		if (length == 0)
		{
			throw std::logic_error("Cannot pop empty array");
		}
		//subtracts first
		auto res= list[(--length)];
		list[length].~T();
		return res;
	}


	template<class T, bool initelems>
	inline T& array<T, initelems>::last()
	{

		if (length == 0)
		{
			throw std::logic_error("Cannot acess the last element of an empty array");
		}
		return list[length-1];
	}

	template<class T, bool initelems>
	inline T& array<T, initelems>::first()
	{
		if (length==0)
		{
			throw std::logic_error("Cannot acess the first element of an empty array");
		}
		return list[0];
	}

	template<class T, bool initelems>
	inline void array<T, initelems>::expand(size_t size)
	{
		resize(resizelength(size));
		length = size;
	}

	template<class T, bool initelems>
	void array<T, initelems >::resize(size_t size) {
		//returns if success


		if (size == 0)
		{
			size = resizelength(length);//default case
		}

		if (size > capacity)//so it cant be shrunk
		{

			T* newlist;
			if constexpr(std::is_trivially_copyable_v<T>)
			{
				if constexpr (std::is_trivially_default_constructible_v<T>) {
					if (capacity == 0)
					{
						newlist = (T*)std::calloc(size,sizeof(T));
					}
					else {
						newlist = (T*)realloc((void*)list, sizeof(T) * size);
						std::memset(newlist + capacity, 0, (size - capacity) * sizeof(T));
					}
					
				}
				else {
					if (capacity == 0)
					{
						newlist = (T*)malloc(sizeof(T) * size);
					}
					else {
						newlist = (T*)realloc((void*)list, sizeof(T) * size);
					}
						for (size_t i = capacity; i < size; i++) {

							new (newlist + i)T();
						}
								
				}
				
			}
			else
			{
				newlist = new T[size];		
				for (size_t i = 0; i < capacity; i++) {

					newlist[i] = std::move(list[i]); 
				
				}for (size_t i = capacity; i < size; i++) {
					new (newlist + i) T();
				}
				if (capacity!=0)
				{

					delete[] list;
				}
				
			}
			
			list = newlist;
			if (!list) {
				throw std::bad_alloc();
			}
			capacity = size;

		}

	}
	

	template<class T, bool initelems>
	void array<T, initelems >::destroy() {
		if (list != nullptr) {
			delete[] list;
			list = nullptr;
		}
		length = 0;
		capacity = 0;
	}

	template<class T, bool initelems >
	void array<T, initelems>::swap(array<T, initelems>& other)
	{
		std::swap(length, other.length);
		std::swap(capacity, other.capacity);
		std::swap(list, other.list);
	}
	
	template<class T, bool initelems >
	array<T, initelems>::array(size_t size) {


		list= 0;
		length = size;
		capacity = 0;
		if (0 < size)
		{
			resize(size);
		}
		

	}

	template<class T, bool initelems>
	inline array<T, initelems>::array(size_t len, const T& value)
	{

		list = nullptr;
		length = len;
		capacity = 0;
		if (0 < length)
		{
			resize(resizelength(length));
		}
		for (size_t i = 0; i < length; i++)
		{
			list[i] = value;
		}
	}




	template<class T, bool initelems >
	constexpr array<T, initelems>::array() noexcept{
		length = 0;
		capacity = 0;
		list = nullptr;
	}

	template<class T, bool initelems >
	inline array<T, initelems>::array(T* arr, size_t size)
	{

		list = nullptr;
		length = size;
		capacity = size;
		list = new T[size];
		if constexpr (std::is_trivially_copyable_v<T>) {
			std::memcpy(list, arr, sizeof(T) * size);  // Fast copy for trivially copyable types
		}
		else {
			for (size_t i = 0; i < size; i++) {
				list[i] = arr[i];
			}
		}

	}
	template<class T, bool initelems>
	inline array<T, initelems>::array(std::initializer_list<T> init)
	{
		capacity = 0;
		list = nullptr;
		length = init.size();
		resize(init.size());

		size_t i = 0;
		for (const auto& val : init) {
			list[i++] = val;
		}
	}
	//just 
	template<class T, bool initelems>
	array<T, initelems>::array(const array& arr) : array(arr.list, arr.length) {


	}
	//takes an r value renrennce,
	template<class T, bool initelems>
	inline array<T, initelems>::array(array&& other) noexcept
	{
		
		length = other.length;
		capacity = other.capacity;
		list = other.list;
		other.list = nullptr;  // Reset the source vector
		other.length = 0;
		other.capacity = 0;

	}
	
	//takes an r value refrence  
	template<class T, bool initelems>
	array<T, initelems>& array<T, initelems>::operator=(array<T, initelems>&& other) noexcept {
		// Prevent self-assignment
		if (this != &other) {
			destroy();
			swap(other);
		}
		return *this;
	}

	template<class T, bool initelems>
	array<T, initelems>& array<T, initelems>::operator=(const array<T, initelems>& other)
	{
		if (this != &other) {
			destroy();
			array temp(other);
			swap(temp);
		}
		return *this;
	}

	template<class T, bool initelems>
	inline bool array<T, initelems>::operator==(const array& other) const
	{
		if (length != other.length)
		{
			return false;
		}
		for (int i = 0; i < length; i++)
		{
			if (other[i] != list[i])
			{
				return false;
			}
		}
		return true;
	}



	template<class T, bool initelems>
	inline size_t array<T, initelems>::find(const T & value) const
	{

		for (size_t i = 0; i < length; ++i) {
			if (list[i] == value) {
				return i;
			}
		}
		return npos;
	}

	template<class T, bool initelems>
	inline bool array<T, initelems>::contains(const T & value) const
	{
		return find(value) != npos;
	}
	template<class T, bool initelems>
	template<class InputIt>
	inline array<T, initelems>::array(InputIt first, InputIt last)
	{
		length = 0;
		capacity = 0;
		list = nullptr;
		auto dist = std::distance(first, last);
		if (dist > 0) {
			expand(dist);
			for (size_t i = 0; first != last; ++first, ++i) {
				list[i] = *first;
			}
		}
	}
	template<class T, bool initelems>
	template<typename... Args>
	inline void array<T, initelems>::push(Args&&... values) {
		constexpr size_t total_new = sizeof...(values);
		if (length + total_new > capacity) {
			resize(resizelength(length + total_new));
		}

		T* dst = list + length;
		size_t i = 0;
		// Use initializer list to ensure left-to-right evaluation
		(void)std::initializer_list<int>{
			((*dst++ = std::forward<Args>(values)), 0)...
		};

		length += total_new;
	}
	
}//newline