//#pragma once
#pragma once 
#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../entities/crystaldaggers.h"

struct playertpcomp : ecs::component
{
	float chargetime;

	float timetilluse;
	const float cooldown = 1;
	void wearduribilty() {
		item* select = owner().get_component<inventory>().selected;
		if (select != nullptr)
		{

			select->amt -= 1;

		}
	}
	void update() {
		timetilluse -= CtxName::ctx.Time->dt;
		item* select = owner().get_component<inventory>().selected;
		if (select == nullptr)
		{
			return;
		}

		
			if (select->id != playertpsword)
			{
				return;
			}
			
			if (chargetime>1)
			{
				if (timetilluse < 0)
				{
					if (CtxName::ctx.Inp->mouseleft().released)
					{
						Vec3 pos =owner().get_component<ecs::transform_comp>().transform.position;
						Vec3 offset =owner().get_component<ecs::transform_comp>().transform.getnormaldirection() * chargetime * chargetime;
						ray moveray = ray(pos, pos + offset);
					voxtra::WorldRayCollision grid=	voxtra::travvox(moveray, voxtra::countall);
					if (!grid)
					{

					 owner().get_component<ecs::transform_comp>().transform.position += offset;

					}
					else
					{
						owner().get_component<ecs::transform_comp>().transform.position = grid.unwrap().intersection();
					}
					owner().get_component<rigidbody>().velocity.y = 0;
						wearduribilty();
						chargetime = 0;
						timetilluse = cooldown;
					}
				}
			}
			if (timetilluse < 0)
			{
				if (CtxName::ctx.Inp->mouseleft().held || CtxName::ctx.Inp->mouseleft().released)
				{
					chargetime += CtxName::ctx.Time->dt;
					chargetime = Min(3, chargetime);
				}
				else {

					chargetime = 0;
				}
			}

			
			
		
	}
	

};



 
