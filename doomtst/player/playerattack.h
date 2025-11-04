#pragma once 
#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../debugger/console.h"

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
		if (!closest.collider.owner.hascomponent<estate>())
		{
			return;
		}
		debug("sees"+std::to_string(CtxName::ctx.Time->dt));
		if (closest.collider.owner.hascomponent<estate>()&&CtxName::ctx.Inp->mouseleft().pressed)
		{
			if (closest.gameobject().hascomponent<rigidbody>())
			{
				kb(owner.transform().position, 7, (closest.gameobject()));
				
			}
			int dmgdone = computeattackdmg();
			closest.gameobject().getcomponent<estate>().damage(dmgdone);
			wearduribilty();
		
		}

	}
	playerattackcomp() {
	
	
	}
	;

};



 // !gameobject_HPP
