#include "../game/ecs/game_object.h"
#include "../game/rigidbody.h"
#include "../game/objecthelper.h"
#pragma once 
struct playerclimb: ecs::component
{

	bool onrope;

	playerclimb() {
		//priority = 111;
		onrope = false;
	}	
	void oncollision(ecs::obj* collidedwith) {
		if (collidedwith->has_component<blockname::block>())
		{
			blockname::block* potrope =collidedwith->get_component_ptr<blockname::block>();
			if (potrope->id==minecraftrope)
			{
				onrope = true;
				owner().get_component<rigidbody>().gravityscale= 0;
			}
		}
	}


	void update() {
		if (onrope==false)
		{

			owner().get_component<rigidbody>().gravityscale = 1;

		}onrope = false;
	}
};

 // !playerclimb_H
