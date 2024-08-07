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
		timetilluse -= timename::dt;
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
					if (userinput::mouseleft.released)
					{
						Vector3 pos = toent(owner).transform.position;
						Vector3 offset = toent(owner).transform.getnormaldirection()*  chargetime * chargetime;
						ray moveray = ray(pos, pos + offset);
					voxtra::RayCollisionWithGrid grid=	voxtra::travvox(moveray, 100, voxtra::countall);
					if (grid.box==nullptr)
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
				if (userinput::mouseleft.held||userinput::mouseleft.released)
				{
					chargetime += timename::dt;
					chargetime = Min(3, chargetime);
				}
				else {

					chargetime = 0;
				}
			}

			
			
		
	}
	

};



#endif 
