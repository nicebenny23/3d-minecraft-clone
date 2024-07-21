#include "gameobject.h"
#include "../util/mathutil.h"
#ifndef  entitystate_HPP
#define entitystate_HPP

struct estate :gameobject::component
{

	int health;
	bool prevonground;
	int maxhealth;
	v3::Vector3 velocitylast;
	void update() {
		velocitylast = owner->getcomponent<rigidbody>().velocity * .03f + velocitylast * .97f;
		testfalldamage();
		prevonground = owner->getcomponent<rigidbody>().isonground;
	}
	void testfalldamage() {
		
		if (owner->getcomponent<rigidbody>().isonground && !prevonground)
		{
			if (velocitylast.y < -13)
			{
				damage(abs(velocitylast.y / 5.f));
			}

		}
	}
	void start() {
		priority = -1111;
		health = maxhealth;
	}
	estate(int maxhp) {
		maxhealth = maxhp;
		health = maxhp;
	}
	void damage(int dmg) {

		health -= dmg;
		health = clamp(health, 1, maxhealth);
	}
};
#endif // ! entitystate_HPP
