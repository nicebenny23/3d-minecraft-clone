#include <iomanip>
#include <iostream>
#include <stdexcept>

#ifndef dynamicarray_HPP
#define dynamicarray_HPP

namespace dynamicarray {

#define resizelen(len) (len==0)?2:2*len
	template<class T>
	class array
	{
	public:

		array();
		array(T* arr, int size);
		T& operator[](int index);
		T getelem(int index);
		
		array(int size);
		void destroy();
		T& at(int ind);
		void append(array arr);
		void append(T * arr,int otherlen);
		bool cutind(int startindex, int endindex);
		void slice(int startindex, int endindex);
		void insertind(int index, T value);
		void merge(int index, const array& arr);
		bool deleteind(int index);
		T& gettop();
		void setlist(array* arr);
		void append(T value);
		T* getdata();
		array(const array& arr);
		int length;
		T* list;
		bool resize(int size = 0);

		int capacity;


	};

	//sets a list to another list while deleting the other said list,creates no memry as deletes fist list 
	template<class T>
	void array<T>::setlist(array* arr) {
		this->destroy();
		this->length = arr->length;
		this->capacity = arr->capacity;
		this->list = new T[this->capacity];



		for (int i = 0; i < this->length; i++) {
			this->list[i] = arr->list[i];
		}
		arr->destroy();
	}

	template<class T>
	T array<T>::getelem(int index) {
		if (index > length) {

			static_assert("index for getelem greater than length ");
		}
		
		if (index < 0)
		{
			static_assert("index for getelem less than 0");
		}
		return list[index];
	}
	template<class T>
	void array<T>::merge(int index, const array& arr) {
		if (index < 0)
		{
			index = 0;
		}
		if (index + arr.length >= length)
		{
			return;
		}
		int otherlength = arr.length;
		if (length + otherlength >= capacity)
		{
			resize(resizelen (length + otherlength));
		}

		for (int i = index; i < length; i++)
		{
			list[i + otherlength] = list[i];
		}
		for (int i = 0; i < otherlength; i++)
		{
			list[i + index] = arr.list[i];
		}
		length += otherlength;
	}
	template<class T>
	T& array<T>::gettop() {
		if (length > 0)
		{
			return list[length - 1];

		}

	}
	//deletes content of list(not pointers!!)
	template<class T>
	void array<T>::destroy() {
		if (list != nullptr) {
			delete[] list;
		}
	}


	template<class T>
	T* array<T>::getdata() {
		//unsafe do not use unless needed
		return list;
	}


	//keeps a range of indices including start and end ind,todo add inclusive exculsive toggle
	template<class T>
	void array<T>::slice(int startindex, int endindex) {


		if (endindex < startindex)
		{
			std::swap(startindex, endindex);
		}
		if (startindex < 0 || endindex >= length)
		{
			return;
		}
		//moves indices 
		for (int i = startindex; i <= endindex; i++)
		{
			list[i - startindex] = list[i];
		}

		// setting length to area remaining
		length = endindex - startindex + 1;

		return;

	}
	//removes a range of indexes,including start/end ind
	template<class T>
	bool array<T>::cutind(int startindex, int endindex) {

		if (endindex < startindex)
		{
			std::swap(startindex, endindex);
		}
		if (startindex < 0 || endindex >= length)
		{
			return false;
		}
		int dif = endindex - startindex;

		for (int i = endindex + 1; i < length; i++)
		{
			list[i - dif - 1] = list[i];
		}


		length -= dif + 1;
		return true;
	}

	//copies an array and uses memory this is used when setting varybles i think
	template<class T>
	array<T>::array(const array& arr) {
		length = arr.length;
		capacity = arr.capacity;
		list = new T[capacity];
		if (list == nullptr)
		{
			delete[] list;

			return;
		}


		for (int i = 0; i < length; i++) {
			list[i] = arr.list[i];
		}

	}
	//copies and appends an element to the list(!!!issues with pointers!!!)
	template<class T>
	void array<T>::append(T value) {
		if (length >= capacity)//sees if wee need more memory due to not enogh space
		{
			resize(resizelen(length));
		}
		list[length] = value;
		length++;

	}
	template<class T>
	bool array<T>::deleteind(int index) {


		if (index < length && index >= 0)//sees if in bounds
		{



			length--;//decrements size
			for (int i = index; i < length; i++)
			{

				list[i] = list[i + 1];//this lets index be removed
			}

			
			return true;
		}
		return false;

	}







	template<class T>
	void array<T>::insertind(int index, T value) {
		if (index < 0)//sees if it over 0
		{
			static_assert("attempted to insert an element in array below zero");
			return;
		}
		if (index > length) {

			static_assert("attempted to insert an element in array greater than length");
			return;
		}
		if (capacity <= length)//sees if wee need more memory due to not enogh space
		{
			resize(resizelen(length));
		}



		for (int i = length; i > index; i--)
		{
			list[i] = list[i - 1];
		}

		list[index] = value;


		if (index >= length)//checks if index is higher than the newsize
		{
			length = index;//sets it to index and while this is wrong it is incremented later
		}
		length++;//increment
	}
	//returns alias to index
	template<class T>
	T& array<T>::operator[](int index) {

		if (index >= capacity)
		{
			if (!resize(resizelen(length)))//array resize failed
			{
				return *list;//to avoid error in case of memory fail and derenfrence it because its cool
			}
		}
		if (index >= length) {//max index is length

			length = index + 1;

		}
		if (index < 0)
		{
			static_assert("index for [] operator less than 0");
		}


		if (length > capacity) {
			if (!resize(resizelen(length)))//array resize failed
			{
				static_assert("a");//again to avoid error in case of memory fail and derenfrence it because its cool
			}

		}

		return list[index];


	}
	//same as [] but only in bounds
	template<class T>
	T& array<T>::at(int index) {
		if (index >= length) {
			static_assert("index for at greater than max length");
		}
		if (index < 0)
		{
			static_assert("index for at less than 0");
		}


		T val = list[index];
		return list[index];


	}
	//appends a list to the end of the list(doesent delete it)(!!!caution with pointer lists!!!)
	template<class T>
	void array<T>::append(array arr) {


		int otherlength = arr.length;
		if (length + otherlength >= capacity)
		{
			resize(resizelen( (length + otherlength)) );
		}


		for (int i = 0; i < otherlength; i++)
		{
			list[i + length] = arr.list[i];
		}
		length += otherlength;
	}
	template<class T>
	inline void array<T>::append(T* arr,int otherlen)
	{
		
		if (length + otherlen>= capacity)
		{
			resize(resizelen( (length + otherlen)));
		}


		for (int i = 0; i < otherlen; i++)
		{
			list[i + length] = arr[i];
		}
		length += otherlen;
	}
	//resizes the array

	template<class T>
	bool array<T>::resize(int size) {
		//returns if success

		if (list == nullptr || length == 0)
		{
		//	length = 0;
		//	*this = array<T>(2);
		//	return true;
		}
		if (size == 0)
		{
			size = 2 * length + 2;//default case
		}

		if (size > capacity)//so it cant be shrunk
		{

			T* newlist = new T[size];


			if (newlist == nullptr)
			{



				return false;
			}

			for (int i = 0; i < length; i++)
			{

				newlist[i] = list[i];
			}

			for (int i = length; i < capacity; i++)
			{

				newlist[i] = T();
			}
			//also the error here is a bug 
			if (list!=nullptr)
			{

				delete[] list;
			}





			list = newlist;

			capacity = size;
			return true;
		}
		return false;
	}

	template<class T>
	array<T>::array(int size) {

		length = 0;
		capacity = size;
		list = new T[size];

		for (int i = 0; i < size; i++)
		{
			list[i] = T();
		}
		if (list == nullptr)
		{
			delete[] list;

			return;
		}

	}




	template<class T>
	array<T>::array() {
		length = 0;
		capacity = 0;
		list = nullptr;





	}

	template<class T>
	inline array<T>::array(T* arr, int size)
	{
		length = size;
		capacity = size;
		list = new T[size];

		for (int i = 0; i < size; i++)
		{
			list[i] = arr[i];
		}
		if (list == nullptr)
		{
			delete[] list;
			static_assert("could not init aray due to lack of memory");
			return;
		}


	}





}
#endif#pragma once
