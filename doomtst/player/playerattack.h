#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#ifndef playeratt_HPP
#define playeratt_HPP
struct playerattackcomp:gameobject::component
{
	void wearduribilty() {
		item* select =owner->getcomponent<inventory>().selected;
		if (select!=nullptr)
		{
			if (select->itemtype == wear) {
				select->amt -= 1;
			}
		}
	}
	int computeattackdmg() {
		

		item* select;
		select = owner->getcomponent<inventory>().selected;
		if (select == nullptr)
		{
			return 1;
		}
		
		return select->properties.dmg;
	}

	void update() {

	
		voxtra::RayWorldIntersection closest;
		ray cameraray = ray(camera::campos(), camera::campos() + camera::GetCamFront() * 7);
		closest = collision::raycastall(cameraray,owner);
		if (closest.collider == nullptr)
		{
			return;
		}
		if (closest.collider->owner->type != gameobject::entity)
		{
			return;
		}
		if (closest.collider->owner->hascomponent<estate>()&&CtxName::ctx.Inp->mouseleft().pressed)
		{
			if (closest.collider->owner->hascomponent<rigidbody>())
			{
				kb(closest.colpoint, 7,& toent(closest.collider->owner));
				
			}
			int dmgdone = computeattackdmg();
			closest.collider->owner->getcomponent<estate>().damage(dmgdone);
			wearduribilty();
		
		}

	}
	playerattackcomp() {
	
	
	}
	;

};



#endif // !gameobject_HPP
