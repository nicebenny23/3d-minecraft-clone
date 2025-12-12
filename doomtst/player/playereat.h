
#include "../game/ecs/game_object.h"
#include "../game/objecthelper.h"
#include "../util/dynamicarray.h"
#include "../items/Container.h"
#include "playerinventory.h"
#include "../player/playerhealth.h"
#include "../items/itemutil.h"
#pragma once 
struct playereat : ecs::component
{

	
	item* food;
	item* prevfood;
	float timetoeat;
	float timeuntilbreak;
	void start() {
		food = nullptr;

	
	}
	bool caneat() {


		if (!CtxName::ctx.Inp->mouseright().held)
		{
			return false;
		}


		
		if (food==nullptr)
		{
			return false;
		}
		if (food->properties.foodval==0)
		{
			return false;
		}
		if (owner().get_component<estate>().health == owner().get_component<estate>().maxhealth)
		{
			return false;
		}
		if (prevfood != food)
		{
			return false;
		}

	
		return true;
	}



	void wearduribilty() {

		if (food != nullptr)
		{
			
				food->amt -= 1;
		
		}
	}



	void eat();
	void tryeat() {
		if (caneat())
		{
			timetoeat -= CtxName::ctx.Time->dt;
			if (timetoeat < 0) {

				timetoeat = 1;
				owner().get_component<estate>().heal(food->amt);
				wearduribilty();
			}
			prevfood = food;
			return;
		}
		timetoeat = 1;
		prevfood =food;
		return;

	}
	void update() {



		food = owner().get_component<inventory>().selected;
		tryeat();
	
	}
};


 // ! playerplace_H
