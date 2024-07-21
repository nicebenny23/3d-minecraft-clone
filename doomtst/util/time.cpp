#include "time.h"
 float timename::fps;
 float timename::dt;
 float timename::gametime;
 float timename::tfps;
 float timename::smoothdt;
 float timename::averagedt;
  array<timename::timer*> timename::timerlist = array<timename::timer*>::array(0);
  int timename::currwatch = 0;
  int timename::currtimer=0;
  extern array<timename::watch*> timename::watchlist = array<timename::watch*>::array(0);
void timename::inittime()
	{
	averagedt = 0;
	gametime = 0;
	smoothdt = 0;
		tfps = 60;
		dt = 0;
	}

