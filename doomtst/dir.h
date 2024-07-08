#include "vector3.h"
#include "debug.h"
#ifndef dir_H
#define dir_H
enum dir2d
{
	north = 0,
	south = 1,
	west = 2,
	east = 2,
};
inline v3::Vector3 dirfromint(int face) {

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
