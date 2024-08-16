#include "../util/time.h"
#ifndef blocktick_HPP
#define blocktick_HPP
#define ticktime .15
namespace tick {

	void trytick();
	extern float tickdt;
	extern bool tickframe;



}

#endif