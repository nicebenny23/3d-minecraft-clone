#include "../game/entity.h"
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#ifndef dmgonhit_HPP
#define dmgonhit_HPP
struct dmgplayeronhit:gameobject::component
{
	int dmgdone;
	dmgplayeronhit(int dmg) {
		priority = 1111;

		dmgdone = dmg;
	}
	void oncollision(gameobject::obj* collidedwith) {

		if (collidedwith->type == gameobject::entity)
		{


			if (collidedwith->hascomponent<playerhealth>())
			{
				v3::Vector3 center = objutil::toent(owner).transform.position;
				v3::Vector3 othercenter = objutil::toent(collidedwith).transform.position;
				if (collidedwith->hascomponent<rigidbody>()) {
					collidedwith->getcomponent<rigidbody>().velocity -= normal(center - othercenter)*7;
				}
				collidedwith->getcomponent<estate>().damage(dmgdone);
			}
		}
	}

};

#endif // !dmgonhit_HPP
