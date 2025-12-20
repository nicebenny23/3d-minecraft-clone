#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../entities/crystaldaggers.h"
#pragma once 
struct playerdaggercomp : ecs::component
{
	void wearduribilty() {
		item* select = owner().get_component<inventory>().selected;
		if (select != nullptr)
		{
		
				select->amt -= 1;
			
		}
	}
	void update() {
		item* select = owner().get_component<inventory>().selected;
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
			Point3 spawpos =owner().get_component<ecs::transform_comp>().transform.position + owner().get_component<ecs::transform_comp>().transform.getnormaldirection() * 3;
			Vec3 velocity =owner().get_component<ecs::transform_comp>().transform.getnormaldirection() * 10;

			spawndagger(spawpos, velocity);
			wearduribilty();
		}
	}
	playerdaggercomp() {


	}
	;

};



 // !ecs_HPP
