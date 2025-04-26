//#pragma once
#ifndef playertp_HPP
#define playertp_HPP
#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../entities/crystaldaggers.h"

struct playertpcomp :gameobject::component
{
	float chargetime;

	float timetilluse;
	const float cooldown = 1;
	void wearduribilty() {
		item* select = owner->getcomponent<inventory>().selected;
		if (select != nullptr)
		{

			select->amt -= 1;

		}
	}
	void update() {
		timetilluse -= CtxName::ctx.Time->dt;
		item* select = owner->getcomponent<inventory>().selected;
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
						Vector3 pos = toent(owner).transform.position;
						Vector3 offset = toent(owner).transform.getnormaldirection()*  chargetime * chargetime;
						ray moveray = ray(pos, pos + offset);
					voxtra::RayWorldIntersection grid=	voxtra::travvox(moveray, 100, voxtra::countall);
					if (grid.collider==nullptr)
					{

						toent(owner).transform.position += offset;

					}
					else
					{
						toent(owner).transform.position = grid.colpoint;
					}
					owner->getcomponent<rigidbody>().velocity.y = 0;
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



#endif 
