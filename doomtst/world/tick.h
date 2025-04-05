#include "../util/time.h"
#ifndef blocktick_HPP
#define blocktick_HPP
constexpr auto ticktime = .15;
namespace tick {

	void trytick();
	extern float tickdt;
	extern bool tickframe;



}

#endif