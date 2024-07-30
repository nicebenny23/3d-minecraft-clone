#include "../game/entity.h"
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#include "../game/entityutil.h"
#ifndef dmgonhit_HPP
#define dmgonhit_HPP
struct destroyonhit :gameobject::component {

	std::string tagtoeffect;
	int dmgdone;
	destroyonhit(int dmg, std::string affecttag) {
		priority = 1111;
		tagtoeffect = affecttag;
		dmgdone = dmg;
	
	}
	void oncollision(gameobject::obj* collidedwith) {

		
			if (tagtoeffect.length() == 0)
			{
				if (!collidedwith->getcomponent<Collider>().effector)
				{
					entityname::destroy(&objutil::toent(owner), false);
				}
				
				return;
			}
		if (collidedwith->type == gameobject::entity)
		{
			if (objutil::toent(collidedwith).hastag(tagtoeffect))
			{
entityname::destroy(&objutil::toent(owner),false);
			}
		}
	}


	destroyonhit() = default;
};
struct dmgonhit:gameobject::component
{
	bool hasknockback;
	std::string tagtoeffect;
	int dmgdone;
	dmgonhit(int dmg,std::string affecttag,bool givekb) {
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

					kb( center, 7,&objutil::toent(collidedwith));

					}
					collidedwith->getcomponent<rigidbody>().inliquid = true;
				}
				collidedwith->getcomponent<estate>().damage(dmgdone);
			
		}
	}

};

#endif // !dmgonhit_HPP
