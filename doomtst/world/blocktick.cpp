#include "blocktick.h"

float tick::tickdt;
bool tick::tickframe;
void tick::trytick() {

	tickframe = false;
	tickdt -= CtxName::ctx.Time->dt;
	if (tickdt<0)
	{
		tickdt = tick::ticktime;
		tickframe = true;
	}

}