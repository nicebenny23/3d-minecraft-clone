#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../entities/crystaldaggers.h"
#pragma once 
struct playerdaggercomp :gameobject::component
{
	void wearduribilty() {
		item* select = owner.getcomponent<inventory>().selected;
		if (select != nullptr)
		{
		
				select->amt -= 1;
			
		}
	}
	void update() {
		item* select = owner.getcomponent<inventory>().selected;
		if (select==nullptr)
		{
			return;
		}
	
		if (CtxName::ctx.Inp->mouseleft().pressed)
		{
			if (select->id!=crystaldaggeritem)
			{
				return;
			}
			Vector3 spawpos =owner.transform().position + owner.transform().getnormaldirection() * 3;
			Vector3 velocity =owner.transform().getnormaldirection() * 10;

			spawndagger(spawpos, velocity);
			wearduribilty();
		}
	}
	playerdaggercomp() {


	}
	;

};



 // !gameobject_HPP
