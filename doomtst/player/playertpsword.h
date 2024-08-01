#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../entities/crystaldaggers.h"
#ifndef playertp_HPP
#define playertp_HPP
struct playertpcomp :gameobject::component
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
		if (select == nullptr)
		{
			return;
		}

		if (userinput::mouseleft.pressed)
		{
			if (select->id != playertpsword)
			{
				return;
			}
			toent(owner).transform.position += toent(owner).transform.getnormaldirection()*3;

			wearduribilty();
		}
	}
	

};



#endif // !gameobject_HPP
