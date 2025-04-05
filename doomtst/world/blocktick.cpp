#include "blocktick.h"

float tick::tickdt;
bool tick::tickframe;
void tick::trytick() {

	tickframe = false;
	tickdt -= timename::dt;
	if (tickdt<0)
	{
		tickdt = tick::ticktime;
		tickframe = true;
	}

}