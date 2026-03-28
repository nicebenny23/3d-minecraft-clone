#include "../game/ecs/game_object.h"
#include "../game/rigidbody.h"

#pragma once 
struct playerclimb: ecs::component
{

	bool onrope;

	playerclimb() {
		//priority = 111;
		onrope = false;
	}
};

 // !playerclimb_H
