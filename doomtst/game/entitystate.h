#include "gameobject.h"
#include "../util/mathutil.h"
#include "entity.h"
#include "objecthelper.h"
#ifndef  entitystate_HPP
#define entitystate_HPP

struct estate :gameobject::component
{
	bool inliquid;
	bool takesfalldmg;
	int health;
	bool prevonground;
	int maxhealth;
	v3::Vector3 velocitylast;

	float invincablilitymax;
	float timetilldmg;
	void update() {
	
		timetilldmg -= timename::smoothdt;
		velocitylast = owner->getcomponent<rigidbody>().velocity * .03f + velocitylast * .97f;
		testfalldamage();
		prevonground = owner->getcomponent<rigidbody>().isonground;
	}
	void testfalldamage() {
		if (takesfalldmg)
		{


			if (owner->getcomponent<rigidbody>().isonground && !prevonground)
			{
				if (velocitylast.y < -10)
				{
					damage(abs(velocitylast.y / 4.f));
				}

			}
		}
	}
	void start() {
		priority = 11;
		health = maxhealth;
	}
	estate(int maxhp,bool falls) {
		maxhealth = maxhp;
		health = maxhp;
		invincablilitymax = 1;
		takesfalldmg = falls;
	}
	void damage(int dmg) {
		if (timetilldmg < 0)
		{

			timetilldmg = invincablilitymax;
			health -= dmg;
			if (health < 0)
			{
				entityname::destroy(&objutil::toent(owner));
			}
			health = clamp(health, 0, maxhealth);
		}
	}
};
#endif // ! entitystate_HPP
