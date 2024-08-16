#include "blocktick.h"

float tick::tickdt;
bool tick::tickframe;
void tick::trytick() {

	tickframe = false;
	tickdt -= timename::smoothdt;
	if (tickdt<0)
	{
		tickdt = tick::ticktime;
		tickframe = true;
	}

}