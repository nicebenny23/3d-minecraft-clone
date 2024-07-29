#include <cmath>
#ifndef mathutil_HPP
#define mathutil_HPP
inline bool aproxequal(float v1, float v2) {

	if (abs(v1 - v2) < .0001)
	{
		return true;
	}
	return false;	
}	


inline float clamp(float val, float low, float high)
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


inline int clamp(int val, int low, int high)
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


inline bool inrange(float val, float low, float high)
{
	if (val > high)
	{
		return false;
	}
	if (val < low)
	{
		return false;
	}
	return true;
}
inline float sigmoid(float v1) {

return	exp(v1) / (1 + exp(v1));
}
inline float smoothmin(float v1,float v2) {
	return v1 + (v2 - v1) / (1 + exp((v1 - v2) / .1f));
	
}
inline float smoothmax(float v1,float v2) {

	return	exp(v1) / (1 + exp(v1));
}

inline float Max(float v1, float v2) {

	if (v1<v2)
	{
		return v2;
	}
	return v1;
}
inline float Min(float v1, float v2) {

	if (v1 < v2)
	{
		return v1;
	}
	return v2;
}
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
