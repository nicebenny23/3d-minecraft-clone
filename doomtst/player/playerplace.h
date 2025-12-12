#pragma once
#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "playerinventory.h"
#include "../items/itemutil.h"


struct playerplace : ecs::component
{
	int curplaceid;
	voxtra::WorldRayCollision Hit;
	item* select;


	float timeuntilbreak;
	void start() {
		select = nullptr;
		curplaceid = 0;
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
		if (!closest.ecs().has_component<blockname::block>())
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
		ray cameraray = ray(owner().get_component<ecs::transform_comp>().transform.position,owner().get_component<ecs::transform_comp>().transform.position +owner().get_component<ecs::transform_comp>().transform.getnormaldirection() * 7);
		block* plamentblock = voxtra::findprevblock(cameraray,voxtra::countsolid);
		if (plamentblock==nullptr)
		{
			return;
		}
		//this must be kept because it can somtimers bug out do to presosion errors;
	
			if (plamentblock->attributes.solid)
			{
				return;
			}
		
		
		if (!Hit)
		{
			return;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();
		Dir::Dir3d dir = Dir::Align(closest.collider.globalbox().center - plamentblock->center());
	
		int blockdirection = Dir::max2ddirection(camera::campos() - closest.Hit.intersectionpoint);

		plamentblock->mesh.direction = Dir::Dir2d(blockdirection);
		plamentblock->mesh.attachdir = dir;
		Box newblockbox = Box(plamentblock->center(), blockscale);
		newblockbox.scale *= .95;
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
		select = owner().get_component<inventory>().selected;
	
		ray cameraray = ray(owner().get_component<ecs::transform_comp>().transform.position, owner().get_component<ecs::transform_comp>().transform.position + owner().get_component<ecs::transform_comp>().transform.getnormaldirection() * 7);
		Hit = collision::raycastall(cameraray, collision::HitQuery(owner()));
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
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<playerplace> = {
		.priority = 3333333
	};
}
