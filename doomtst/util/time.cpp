#include "time.h"
 float timename::fps;
 float timename::dt;
 float timename::gametime;
 float timename::tfps;
 float timename::smoothdt;
 float timename::averagedt;
void timename::inittime()
	{
	averagedt = 0;
	gametime = 0;
	smoothdt = 0;
		tfps = 60;
		dt = 0;
	}

