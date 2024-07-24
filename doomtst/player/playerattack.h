#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
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
		
		return select->dmg;
	}

	void update() {

	
		voxtra::RayCollisionWithGrid closest;
		ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
		closest = collision::raycastall(cameraray);
		if (closest.box == nullptr)
		{
			return;
		}
		if (closest.box->owner->type != gameobject::entity)
		{
			return;
		}
		if (closest.box->owner->hascomponent<estate>()&&userinput::mouseleft.pressed)
		{
			int dmgdone = computeattackdmg();
			closest.box->owner->getcomponent<estate>().damage(dmgdone);
			wearduribilty();
			if (closest.box->owner->hascomponent<rigidbody>())
			{
				
				closest.box->owner->getcomponent<rigidbody>().velocity-= normal(toent(owner).transform.position-closest.colpoint)*8;
			}
		}

	}
	playerattackcomp() {
	
	
	}
	;

};



#endif // !gameobject_HPP
