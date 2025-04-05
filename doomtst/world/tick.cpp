#include "tick.h"

	float tick::tickdt = 0;
	bool tick::tickframe = 0;
	void tick::trytick() {
		if (tickframe)
		{
			tickdt = 0;
		}

		tickframe = false;
		tickdt += timename::dt;
		if (tickdt >ticktime)
		{
			
			tickframe = true;
		}

	}
