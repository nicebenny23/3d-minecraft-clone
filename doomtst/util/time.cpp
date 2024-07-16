#include "time.h"
 float timename::fps;
 float timename::dt;
 float timename::gametime;
 float timename::tfps;
 float timename::mindt;
  array<timename::timer*> timename::timerlist = array<timename::timer*>::array(0);
  int timename::currwatch = 0;
  int timename::currtimer=0;
  extern array<timename::watch*> timename::watchlist = array<timename::watch*>::array(0);
void timename::inittime()
	{
	gametime = 0;
		dt = 0;
		tfps = 60;
		dt = 0;
	}

