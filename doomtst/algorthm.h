
#pragma once
#include <cmath>
namespace oalgorithm {



	template<typename T>
	bool sorted(T* arr, int start, int end) {
		int prev = arr[start];
		for (int i = start + 1; i < end; i++)
		{
			if (arr[i] >= prev)
			{
				prev = arr[i];
			}
			else
			{
				return false;
			}
		}
		return true;
	}
	template<typename T>
	int linsearch(T* arr, int low, int high, T searchval) {


		for (int i = low; i < high; i++)
		{
			if (arr[i] == searchval) {


				return i;
			}
		}
		return -1;
	}
	template<typename T>
	int binindexsearch(T* arr, int low, int high, T searchval) {

		int index = floor((low + high) / 2);
		while (arr[index] != searchval)
		{

			if (arr[index] > searchval)
			{
				high = index - 1;

			}
			else
			{
				low = index + 1;
			}
			index = floor((low + high) / 2);
			if (low > high)
			{
				return -1;
			}
		}
		return index;


	}

	template <typename T>
	T clamp(T val, int low, int high)
	{
		if (val > high)
		{
			return high;
		}
		if (val < low)
		{
			return low;
		}
		return val;
	}
	//todo low priority redo
	template <typename T>
	int parttion(T* arr, int low, int high)
	{
		T pivot = arr[int(floor((high + low) / 2))];
		int leftind = low - 1;
		int rightind = high + 1;

		while (true)
		{

			do
			{
				leftind++;
			} while (arr[leftind] < pivot);
			do
			{
				rightind--;
			} while (arr[rightind] > pivot);

			if (leftind >= rightind)
			{
				return rightind;
			}
			T carry = arr[leftind];
			arr[leftind] = arr[rightind];
			arr[rightind] = carry;

		}

	}


	template <typename T>
	void quicksub(T* arr, int low, int high) {

		if (low < high)
		{
			int p = parttion(arr, low, high);

			quicksub(arr, low, p);
			quicksub(arr, p + 1, high);
		}



	}



	template <typename T>
	void quicksort(T* arr, int len) {
		quicksub(arr, 0, len - 1);
	}


	template <typename T>
	int parttionptr(T* arr, int low, int high, bool (*lt)(T, T), bool (*gt)(T, T))
	{
		T pivot = arr[int(floor((high + low) / 2))];
		int leftind = low - 1;
		int rightind = high + 1;

		while (true)
		{

			do
			{
				leftind++;
			} while (lt(arr[leftind], pivot));
			do
			{
				rightind--;
			} while (gt(arr[rightind], pivot));

			if (!lt(leftind, rightind))
			{
				return rightind;
			}
			T carry = arr[leftind];
			arr[leftind] = arr[rightind];
			arr[rightind] = carry;

		}

	}


	template <typename T>
	void quicksubptr(T* arr, int low, int high, bool (*lt)(T, T), bool (*gt)(T, T)) {

		if (low < high)
		{
			int p = parttion(arr, low, high, lt, gt);

			quicksub(arr, low, p, lt, gt);
			quicksub(arr, p + 1, high, lt, gt);
		}



	}



	template <typename T>
	void quicksortptr(T* arr, int len, bool (*gt)(T, T), bool (*lt)(T, T)) {
		quicksub(arr, 0, len - 1, lt, gt);
	}



}