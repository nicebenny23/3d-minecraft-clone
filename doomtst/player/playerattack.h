#pragma once 
#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../debugger/console.h"

struct playerattackcomp: ecs::component
{
	void wearduribilty() {
		item* select =owner().get_component<inventory>().selected;
		if (select!=nullptr)
		{
			if (select->itemtype == wear) {
				select->amt -= 1;
			}
		}
	}
	int computeattackdmg() {
		

		item* select;
		select = owner().get_component<inventory>().selected;
		if (select == nullptr)
		{
			return 1;
		}
		
		return select->properties.dmg;
	}

	void update() {

	
		ray cameraray = ray(camera::campos(), camera::campos() + camera::GetCamFront() * 7);
		
		voxtra::WorldRayCollision Hit = collision::raycastall(cameraray, collision::HitQuery(owner()));
		if (!Hit)
		{
			return;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();
		if (!closest.collider.owner().has_component<estate>())
		{
			return;
		}
		debug("sees"+std::to_string(CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt));
		if (closest.collider.owner().has_component<estate>()&&CtxName::ctx.Inp->mouseleft().pressed)
		{
			if (closest.ecs().has_component<rigidbody>())
			{
				kb(owner().get_component<ecs::transform_comp>().transform.position, 7, (closest.ecs()));
				
			}
			int dmgdone = computeattackdmg();
			closest.ecs().get_component<estate>().damage(dmgdone);
			wearduribilty();
		
		}

	}
	playerattackcomp() {
	
	
	}
	;

};



 // !ecs_HPP
