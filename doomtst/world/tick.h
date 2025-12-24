#include "../game/time.h"
#pragma once 
constexpr auto ticktime = .15;
namespace tick {

	void trytick();
	
	extern float tickdt;
	extern bool tickframe;
	


}

