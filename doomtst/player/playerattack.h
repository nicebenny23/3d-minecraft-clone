#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#pragma once 
struct playerattackcomp: gameobject::component
{
	void wearduribilty() {
		item* select =owner.getcomponent<inventory>().selected;
		if (select!=nullptr)
		{
			if (select->itemtype == wear) {
				select->amt -= 1;
			}
		}
	}
	int computeattackdmg() {
		

		item* select;
		select = owner.getcomponent<inventory>().selected;
		if (select == nullptr)
		{
			return 1;
		}
		
		return select->properties.dmg;
	}

	void update() {

	
		ray cameraray = ray(camera::campos(), camera::campos() + camera::GetCamFront() * 7);
		
		voxtra::WorldRayCollision Hit = collision::raycastall(cameraray, collision::HitQuery(owner));
		if (!Hit)
		{
			return;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();
		if (!closest.collider->owner.hascomponent<estate>())
		{
			return;
		}
		if (closest.collider->owner.hascomponent<estate>()&&CtxName::ctx.Inp->mouseleft().pressed)
		{
			if (closest.collider->owner.hascomponent<rigidbody>())
			{
				kb(closest.Hit.intersectionpoint, 7, (closest.collider->owner));
				
			}
			int dmgdone = computeattackdmg();
			closest.collider->owner.getcomponent<estate>().damage(dmgdone);
			wearduribilty();
		
		}

	}
	playerattackcomp() {
	
	
	}
	;

};



 // !gameobject_HPP
