#pragma once

#include "../debugger/debug.h"
#include <stdexcept>

#include <new>
#include <cstdlib>
#include <cstring>
#pragma once

namespace Cont {


	// Resizing function that doubles the size of the array when needed.
	// If the length is 0, it initializes to a default size of 2.
	inline size_t resizelength(size_t length) {
		return (length == 0) ? 2 : 2 * length;  // Double the capacity for efficient reallocation
	}

	template<class T, bool initelems = true>
	class array {
	public:
		// Debugging function to prsize_t the contents of the list (useful for debugging).
		void debuglist();


		bool empty();
		// Standard operator[] that returns a reference to the element at the given index.
		T& operator[](size_t index);
		const T& operator[](size_t index) const;
		// At function that checks if the index is in range before returning the element.
		T& at(size_t index);

		// Unsafe method to directly access an element at the specified index.
		T& UncheckedAt(const size_t index);

		// Cut a range of elements from startindex to endindex.
		void cutind(size_t startindex, size_t endindex);

		// Slice the array between a start and end index.
		void slice(size_t startindex, size_t endindex);

		// Insert an element at a specific index, shifting others.
		void insertind(size_t index, T value);

		// Delete an element at the given index, shifting others.
		void deleteind(size_t index);




		// Append a single value to the array.
		void push(const T& value);

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
		array();

		// Constructor that initializes the array with a given size.
		array(size_t size);

		// Constructor that initializes the array from a raw posize_ter and size.
		array(T* arr, size_t size);

		// Move constructor.
		array(array&& other) noexcept;

		// Copy constructor.
		explicit array(const array& arr);

		// Move assignment operator.
		array& operator=(array&& other) noexcept;
		array& operator=(const array& other);
		bool operator==(const array& other) const;
		// Posize_ter to the dynamically allocated array.
		T* list;
		// Current number of elements in the array.
		size_t length;

		// Total allocated capacity of the array.
		size_t capacity;

		// Iterator class for accessing elements using range-based for loops.
		class Iterator {
		private:
			T* ptr;  // Posize_ter to the current element in the array.

		public:
			using value_type = T;
			using iterator = Iterator;
			using const_iterator = const T*;
			using size_type = std::size_t;
			// Constructor initializes the iterator with a posize_ter.
			Iterator(T* p) : ptr(p) {}

			// Dereference operator to return the element posize_ted to by the iterator.
			T& operator*() {
				return *ptr;
			}

			// Pre-increment operator to move to the next element.
			Iterator& operator++() {
				++ptr;
				return *this;
			}

			// Comparison operator to check if two iterators are not equal.
			bool operator!=(const Iterator& other) const {
				return ptr != other.ptr;
			}
			bool operator==(const Iterator& other) const {
				return ptr == other.ptr;
			}


		};

		// Methods to get the beginning and end iterators for range-based for loops.
		Iterator begin() {
			return Iterator(list);  // Return an iterator to the first element.
		}

		Iterator end() {
			return Iterator(list + length);  // Return an iterator past the last element.
		}
		using iterator = Iterator;

		// Method to return the current capacity of the array.
		size_t get_size() const {
			return capacity;
		}

		// A method that checks if a value is undefined (default-constructed).
		bool is_undefined(const T& value) const {
			return value == T();  // Assuming that the default constructor of T indicates an undefined value.
		}
		void resize(size_t size = 0);

	};




	template<class T, bool initelems>
	inline void array<T, initelems >::debuglist()
	{
		for (size_t i = 0; i < length; i++)
		{
			debug(list[i]);
		}
	}

	//deletes content of list(not posize_ters!!)

	template<class T, bool initelems>
	inline bool array<T, initelems>::empty()
	{
		return (length == 0);
	}

	//returns alias to index
	template<class T, bool initelems >
	T& array<T, initelems>::operator[](size_t index) {

		if (index >= capacity)
		{
			resize(resizelength(index));

		}
		if (index >= length) {//max index is length
			//i do not make it an error because this funciton is supposed to go out of bonds
			length = index + 1;

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
	//unsafe
	template<class T, bool initelems>
	inline T& array<T, initelems>::UncheckedAt(const size_t ind)
	{

		return list[ind];
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
	template<class T, bool initelems>
	void array<T, initelems>::push(const T& value) {
		if (length >= capacity)
		{
			resize(resizelength(length));
		}
		list[length] = T(value);
		length++;

	}
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
		T store = list[length - 1];
		length -= 1;
		return store;

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

			void* newlist;
			if (capacity != 0)
			{
				//reealloc because it is faster.
				newlist = realloc((void*)list, sizeof(T) * size);
			}
			else
			{

				newlist = malloc(sizeof(T) * size);
			}

			if (!newlist) {
				throw std::bad_alloc();
			}
			list = static_cast<T*>(newlist);
			//somtimes dont want to init elems so i make it an option to turn it off
			if (initelems)
			{
				for (size_t i = capacity; i < size; i++) {

					new (list + i)T();
				}
			}

			//


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


		length = 0;
		capacity = 0;
		if (0 < size)
		{
			resize(size);
		}
		else
		{
			list = nullptr;
		}

	}




	template<class T, bool initelems >
	array<T, initelems>::array() {
		length = 0;
		capacity = 0;
		list = nullptr;
	}

	template<class T, bool initelems >
	inline array<T, initelems>::array(T* arr, size_t size)
	{
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



}