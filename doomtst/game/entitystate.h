#include "gameobject.h"

struct estate
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
	}
};