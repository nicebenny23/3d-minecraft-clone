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
	float knockback;
	std::string tagtoeffect;
	int dmgdone;
	dmgonhit(int dmg,std::string affecttag,float kb=0) {
		priority = 1111;
		tagtoeffect=affecttag ;
	dmgdone = dmg;
	knockback = kb;
		
	}
	void oncollision(gameobject::obj* collidedwith) {

		if (collidedwith->type == gameobject::entity)
		{
			if (!collidedwith->hascomponent<estate>()) {
			
				return;
			}
			if (!objutil::toent(collidedwith).hastag(tagtoeffect))
			{
				return;
			}
			v3::Vector3 center = objutil::toent(owner).transform.position;
				v3::Vector3 othercenter = objutil::toent(collidedwith).transform.position;
				if (collidedwith->hascomponent<rigidbody>()) {
				

					kb( center, knockback,&objutil::toent(collidedwith));

					
					
				}
				collidedwith->getcomponent<estate>().damage(dmgdone);
			
		}
	}

};

#endif // !properties.dmgonhit_HPP
