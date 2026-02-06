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
		stn::Option<items::item_stack&> select =owner().get_component<inventory>().selected();
		if (select)
		{
			if (select.unwrap().owner().has_component<items::item_durability>()) {
				select.unwrap().owner().get_component<items::item_durability>().use();
			}
		}
	}
	int computeattackdmg() {
		return 1;
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
		if (closest.collider.owner().has_component<estate>()&& world().get_resource<userinput::InputManager>().unwrap().left_mouse().pressed)
		{
			if (closest.owner().has_component<rigidbody>())
			{
				kb(owner().get_component<ecs::transform_comp>().transform.position, 7, (closest.owner()));
				
			}
			int dmgdone = computeattackdmg();
			closest.owner().get_component<estate>().damage(dmgdone);
			wearduribilty();
		
		}

	}
	playerattackcomp() {
	
	
	}
	;

};



 // !ecs_HPP
