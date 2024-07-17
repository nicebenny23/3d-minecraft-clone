#include "player.h"
#include "../rigidbody.h"
using namespace player;
entityname::entityref player::goblin;

void player::initplayer()
{
	goblin = entityname::createentity(Vector3(1,10,0), "");
	goblin.toent()->addcomponent<estate>(10);
	goblin.toent()->addcomponent<playerhealth>();

		goblin.toent()->addcomponent<colrect>(zerov, unitv / 2, true);
	goblin.toent()->addcomponent<playermovement>();
	goblin.toent()->addcomponent< playermod>();
	goblin.toent()->addcomponent<rigidbody>();
	goblin.toent()->getcomponent <colrect>().hasrigidbody = true;
}
