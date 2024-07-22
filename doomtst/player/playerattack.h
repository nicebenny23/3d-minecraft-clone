#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#ifndef playeratt_HPP
#define playeratt_HPP
struct playerattackcomp:gameobject::component
{
	void update() {
		voxtra::RayCollisionWithGrid closest;
		ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
		closest = collision::raycastall(cameraray);
		if (closest.box == nullptr)
		{
			return ;
		}
		if (closest.box->owner->type != gameobject::entity)
		{
			return ;
		}
		if (closest.box->owner->hascomponent<estate>())
		{
			closest.box->owner->getcomponent<estate>().damage(3);
		}
	}
	playerattackcomp() {
	
	
	}
	;

};



#endif // !gameobject_HPP
