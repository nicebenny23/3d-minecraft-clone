
#include "algorthm.h"


template<typename T>
void oalgorithm::quicksubptr(T* arr, int low, int high, bool(*lt)(T, T), bool(*gt)(T, T))
{
	if (low < high)
	{
		int p = parttion(arr, low, high, lt, gt);

		quicksub(arr, low, p, lt, gt);
		quicksub(arr, p + 1, high, lt, gt);
	}



}

float oalgorithm::invertnoisetransform(stn::array<float>& arr, float searchval)
{
	int high = arr.length() - 1;
	int low = 0;
	int index = ((low + high) / 2);
	while (true)
	{

		if (arr.unchecked_at(index)> searchval)
		{
			high = index - 1;

		}
		else
		{
			low = index + 1;
		}
		index = (low + high) / 2;
		if (low >= high)
		{
return (float(index * 2) / float(arr.length())) - 1.f;
		}
	}
}
