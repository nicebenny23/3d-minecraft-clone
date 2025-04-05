#include "vector3.h"
#include "../debugger/debug.h"
#ifndef dir_H
#define dir_H

enum dir3d
{
	north3d = 0,
	south3d = 1,
	west3d = 2,
	east3d = 3,
	front3d=4,
	back3d=5,
};
enum dir2d
{
	west2d = 0,
	east2d = 1,

	front2d = 4,
	back2d = 5,
};
inline int invdirind(int dir) {

	return dir + 1 - 2 * modabs(dir, 2);
}
inline int maxdirection(v3::Vector3 point) {


	float max = Max(abs(point.x), Max(abs(point.y), abs(point.z)));
	if (abs(point.x)==max)
	{
		return (1-sign(point.x))/2;
	}
	if (abs(point.y) == max)
	{
		return 2+(1 - sign(point.y)) / 2;
	}
	if (abs(point.z) == max)
	{
		return 4 + (1 - sign(point.z)) / 2;
	}
}
inline int max2ddirection(v3::Vector3 point) {


	float max = Max(abs(point.x),  abs(point.z));
	if (abs(point.x) == max)
	{
		return ( 1- sign(point.x)) / 2;
	}
	
	if (abs(point.z) == max)
	{
		return 4 + (1 - sign(point.z)) / 2;
	}
}
inline v3::Vector3 dirfromint(const int face) {

	switch (face)
	{
	case 0:
		return  v3::Vector3(1, 0, 0);//left
	case 1:
		return v3::Vector3(-1, 0, 0);//right
	
	case 2:
		return v3::Vector3(0, 1, 0);//north
	case 3:
		return v3::Vector3(0, -1, 0);//south
	case 4:
		return v3::Vector3(0, 0, 1);//front
	case 5:
		return v3::Vector3(0, 0, -1);//back
	
	
	default:
		Assert("atempetd to acess a nonexistant direction");

		break;
	}


}

#endif // !dir_H
