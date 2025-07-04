#pragma once
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "playerinventory.h"
#include "../items/itemutil.h"


struct playerplace :gameobject::component
{
	int curplaceid;
	voxtra::WorldRayCollision Hit;
	item* select;


	float timeuntilbreak;
	void start() {
		select = nullptr;
		curplaceid = 0;
		priority = 101010101;
	}
	bool caninteract() {

		if (select == nullptr)
		{
			return false;
		}


		if (!Hit)
		{
			return false;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();
		if (closest.collider->owner.type() != gameobject::block)
		{
			return false;
		}
		if (!inrange(closest.Hit.dist, -1, interactmaxrange))
		{
			return false;
		}
		return true;
	}
	void placeblock() {
		ray cameraray = ray(owner.transform().position,owner.transform().position +owner.transform().getnormaldirection() * 7);
		block* plamentblock = voxtra::findprevblock(cameraray, 1000,voxtra::countsolid);
		//this must be kept because it can somtimers bug out do to presosion errors;
		if (plamentblock!=nullptr)
		{

			if (plamentblock->attributes.solid)
			{
				return;
			}
		}

		voxtra::RayWorldHit closest = Hit.unwrap();
		Dir::Dir3d dir = Dir::Align(closest.collider->globalbox().center - plamentblock->center());
	
		int blockdirection = Dir::max2ddirection(camera::campos() - closest.Hit.intersectionpoint);

		plamentblock->mesh.direction = blockdirection;
		plamentblock->mesh.attachdir = dir;
		Box newblockbox = Box(plamentblock->center(), blockscale);
		bool collides = collision::boxCollidesWithEntity(newblockbox,collision::HitQuery());
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
		select = owner.getcomponent<inventory>().selected;
	
		ray cameraray = ray(owner.transform().position, owner.transform().position + owner.transform().getnormaldirection() * 7);
		Hit = collision::raycastall(cameraray, collision::HitQuery(owner));
		if (!caninteract())
		{
			return;
		}



	

		if (CtxName::ctx.Inp->mouseright().pressed)
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
