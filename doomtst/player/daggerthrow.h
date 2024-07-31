#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../entities/crystaldaggers.h"
#ifndef playerdagger_HPP
#define playerdagger_HPP
struct playerdaggercomp :gameobject::component
{
	void wearduribilty() {
		item* select = owner->getcomponent<inventory>().selected;
		if (select != nullptr)
		{
		
				select->amt -= 1;
			
		}
	}
	void update() {
		item* select = owner->getcomponent<inventory>().selected;
		if (select==nullptr)
		{
			return;
		}
	
		if (userinput::mouseleft.pressed)
		{
			if (select->id!=crystaldaggeritem)
			{
				return;
			}
			Vector3 spawpos = toent(owner).transform.position + toent(owner).transform.getnormaldirection()*3;
			Vector3 velocity = toent(owner).transform.getnormaldirection() * 10;

			spawndagger(spawpos, velocity);
			wearduribilty();
		}
	}
	playerdaggercomp() {


	}
	;

};



#endif // !gameobject_HPP
