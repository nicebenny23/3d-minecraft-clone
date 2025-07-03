#include "../game/entity.h"
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#include "../game/entityutil.h"
#pragma once 
template<typename T,bool is_not_tag =false>
struct destroyonhit :gameobject::component {

	

	destroyonhit() {
		priority = 1111;
		utype == gameobject::updatenone;
	}
	void oncollision(gameobject::obj* collidedwith) {

		
		
		if (collidedwith->type() == gameobject::entity)
		{
			if (collidedwith->hascomponent<T>()^is_not_tag)
			{
				owner->deffered_destroy();
			}
		}
	}

	~destroyonhit() = default;
};
template<typename T>

struct dmgonhit:gameobject::component
{

	
	float knockback;

	int dmgdone;
	dmgonhit(int dmg,float kb=0) {
	priority = 1111;
	
	dmgdone = dmg;
	knockback = kb;
	utype = gameobject::updatenone;
	
	}
	void oncollision(gameobject::obj* collidedwith) {

		
		if (collidedwith->type() == gameobject::entity)
		{
			if (!collidedwith->hascomponent<estate>()) {
			
				return;
			}
			if (!(collidedwith->hascomponent<T>()))
			{
				return;
			}
			v3::Vector3 center = owner->transform().position;
				v3::Vector3 othercenter = collidedwith->transform().position;
				if (collidedwith->hascomponent<rigidbody>()) {
				

					kb( center, knockback,*collidedwith);

					
					
				}
				collidedwith->getcomponent<estate>().damage(dmgdone);
			
		}
	}
	~dmgonhit() = default;
};

 // !properties.dmgonhit_HPP
