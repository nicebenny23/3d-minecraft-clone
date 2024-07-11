#include <cmath>
#ifndef mathutil_HPP
#define mathutil_HPP

inline int modabs(int x, int m) {

	if (x < 0)
	{
		if (static_cast<double>(x) / m == int(x / m))
		{
			//so (-16,-16) is now a part of the correct chunk this has to bedon due to the inverse taken
			return 0;
		}
		else {
			return m - ((-x) % m);
		}
	}
	else
	{
		return x % m;
	}
}

inline float floorabs(float x) {

	if (x < 0)
	{
		return -1 * floor(-1 * x);
	}
	else
	{
		return floor(x);
	}
}
inline int ceilabs(float x) {

	if (x < 0)
	{
		return -1 * ceil(-1 * x);
	}
	else
	{
		return ceil(x);
	}
}
inline int sign(float x) {

	if (x < 0)
	{
		return -1;
	}
	if (0<=x)
	{
		return 1;
	}

}
#endif // !mathutil
