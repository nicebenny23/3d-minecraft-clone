
#include "../game/gameobject.h"
#include "../game/objecthelper.h"
#include "../util/dynamicarray.h"
#include "../items/Container.h"
#include "playerinventory.h"
#include "../player/playerhealth.h"
#include "../items/itemutil.h"
#pragma once 
struct playereat :gameobject::component
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
		if (owner.getcomponent<estate>().health == owner.getcomponent<estate>().maxhealth)
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
				owner.getcomponent<estate>().heal(food->amt);
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



		food = owner.getcomponent<inventory>().selected;
		tryeat();
	
	}
};


 // ! playerplace_H
