#include "../renderer/uibox.h"
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../items/Container.h"
#include "playerinventory.h"
#include "../items/itemutil.h"
using namespace objutil;
#ifndef  playerplace_HPP
#define  playerplace_HPP
struct playerplace :gameobject::component
{
	int curplaceid;
	voxtra::RayCollisionWithGrid closest;
	item* select;


	float timeuntilbreak;
	void start() {
		select = nullptr;
		curplaceid = 0;
		closest.box = nullptr;
		priority = 101010101;
	}
	bool caninteract() {

		if (select == nullptr)
		{
			return false;
		}


		if (closest.box == nullptr)
		{
			return false;
		}
		if (closest.box->owner->type != gameobject::block)
		{
			return false;
		}
		if (!inrange(closest.dist, -1, interactmaxrange))
		{
			return false;
		}
		return true;
	}
	void placeblock() {
		ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
		block* plamentblock = voxtra::findprevblock(cameraray, 1000,voxtra::countsolid);
		//this must be kept because it can somtimers bug out do to presosion errors;
		if (plamentblock!=nullptr)
		{

			if (plamentblock->attributes.solid)
			{
				return;
			}
		}
		int dir = maxdirection(closest.box->box.center - plamentblock->center());
	
		int blockdirection = max2ddirection(Vector3(camera::campos) - closest.colpoint);

		plamentblock->mesh.direction = blockdirection;
		plamentblock->mesh.attachdir = dir;
		Box newblockbox = Box(plamentblock->center(), unitscale);
		bool collides = collision::boxCollidesWithEntity(newblockbox);
		if (!collides)
		{
			int placeid = blockidfromitemid(select);
			if (placeid==-1)
			{
				return;
			}
			gridutil::setblock(plamentblock->pos, placeid);
	
			select->use(1);
		}




	}


	void update() {

		if (ismenuopen())
		{
			return;
		}
		select = owner->getcomponent<inventory>().selected;

		ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
		closest = collision::raycastall(cameraray,owner);
		if (!caninteract())
		{
			return;
		}



	

		if (userinput::mouseright.pressed)
		{
			
				if (blockidfromitemid(select) != -1)
				{
					if (select->canuse(1))
					{

						placeblock();
					}

				}
		}

	}



	
};


#endif // ! playerplace_H
