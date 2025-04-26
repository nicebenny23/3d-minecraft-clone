#include "tick.h"
#include "../game/GameContext.h"
	float tick::tickdt = 0;
	bool tick::tickframe = 0;
	void tick::trytick() {
		if (tickframe)
		{
			tickdt = 0;
		}

		tickframe = false;
		tickdt += CtxName::ctx.Time->dt;
		if (tickdt >ticktime)
		{
			
			tickframe = true;
		}

	}
