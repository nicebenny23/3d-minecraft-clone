#include "gameobject.h"
#include "../util/mathutil.h"
#include "entity.h"
#include "rigidbody.h"
#include "objecthelper.h"
#pragma once
struct entityeffects {
	float timetilllavaover;

};                 

struct estate : gameobject::component
{
	entityeffects effects;
	
	bool inliquid;
	bool takesfalldmg;
	int health;
	bool prevonground;
	int maxhealth;
	float damagemultiplyer;
	v3::Vector3 velocitylast;
	float lastongroundy;
	float invincablilitymax;
	float timetilldmg;
	void update() {

		timetilldmg -= CtxName::ctx.Time->dt;
		if (owner.hascomponent<rigidbody>())
		{
			velocitylast = owner.getcomponent<rigidbody>().velocity * .03f + velocitylast * .97f;
			testfalldamage();
			prevonground = owner.getcomponent<rigidbody>().isonground;
		}
		
	}
	void testfalldamage() {
		if (takesfalldmg)
		{


			if (owner.getcomponent<rigidbody>().isonground && !prevonground)
			{
				float ypos = owner.transform().position.y;
				float falldmg = Max(3.f, lastongroundy - ypos) - 3.f;
				if (owner.getcomponent<rigidbody>().velocity.y<-5)
				{

					damage((falldmg/100));

				}

			}
			if (owner.getcomponent<rigidbody>().isonground) {


				lastongroundy = owner.transform().position.y;
			}
		}
	}
	void start() {
		priority = 11;
		health = maxhealth;
		lastongroundy = owner.transform().position.y;
	}
	estate(int maxhp, bool falls) {
		maxhealth = maxhp;
		health = maxhp;
		damagemultiplyer = 1;
		invincablilitymax = 1;
		takesfalldmg = falls;
	}
	void damage(int dmg) {
		dmg = dmg * damagemultiplyer;
		if (dmg <= 0)
		{
			return;
		}
		if (timetilldmg < 0)
		{

			timetilldmg = invincablilitymax;
			health -= dmg;
			if (health <= 0)
			{
				if (!owner.hascomponent<gameobject::StaticComponent>())
				{

					owner.deffered_destroy();
				}
				
			}
			health = clamp(health, 0, maxhealth);
		}


	}
	void heal(int healamt) {
		if (healamt <= 0)
		{
			return;
		}

	timetilldmg = invincablilitymax;
		health += healamt;

		health = clamp(health, 0, maxhealth);

	}
};
 // ! entitystate_HPP;
