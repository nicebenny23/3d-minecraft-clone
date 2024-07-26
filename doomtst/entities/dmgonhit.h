#include "../game/entity.h"
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#ifndef dmgonhit_HPP
#define dmgonhit_HPP
struct dmgplayeronhit:gameobject::component
{
	bool hasknockback;
	std::string tagtoeffect;
	int dmgdone;
	dmgplayeronhit(int dmg,std::string affecttag,bool givekb) {
		priority = 1111;
		tagtoeffect=affecttag ;
		dmgdone = dmg;
		hasknockback = givekb;
	}
	void oncollision(gameobject::obj* collidedwith) {

		if (collidedwith->type == gameobject::entity)
		{

			if (!objutil::toent(collidedwith).hastag(tagtoeffect))
			{
				return;
			}
			v3::Vector3 center = objutil::toent(owner).transform.position;
				v3::Vector3 othercenter = objutil::toent(collidedwith).transform.position;
				if (collidedwith->hascomponent<rigidbody>()) {
					if (hasknockback)
					{

						collidedwith->getcomponent<rigidbody>().velocity -= normal(center - othercenter) * 7;

					}
					collidedwith->getcomponent<rigidbody>().inliquid = true;
				}
				collidedwith->getcomponent<estate>().damage(dmgdone);
			
		}
	}

};

#endif // !dmgonhit_HPP
