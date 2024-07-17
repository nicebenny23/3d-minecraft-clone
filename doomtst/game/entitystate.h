#include "gameobject.h"
#include "../util/mathutil.h"
#ifndef  entitystate_HPP
#define entitystate_HPP

struct estate :gameobject::component
{

	int health;
	int maxhealth;
	void update() {


	}
	void start() {

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
