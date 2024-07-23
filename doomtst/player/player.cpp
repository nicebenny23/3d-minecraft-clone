#include "player.h"
#include "../game/rigidbody.h"
#include "playerinventory.h"
#include "playerattack.h"
#include "playerplace.h"
#include "playermodification.h"
using namespace player;
entityname::entityref player::goblin;

void player::initplayer()
{
	goblin = entityname::createentity(Vector3(0,5,0), "");
	goblin.toent()->addcomponent<estate>(10,true);
	goblin.toent()->addcomponent<playerhealth>();
	goblin.toent()->addcomponent<inventory>();
		goblin.toent()->addcomponent<Collider>(zerov, unitv / 2, true);
	goblin.toent()->addcomponent<playermovement>();
	goblin.toent()->addcomponent< playerbreak>();
	goblin.toent()->addcomponent< playerplace>();
	goblin.toent()->addcomponent<rigidbody>();
	goblin.toent()->addcomponent<playerattackcomp>();
	goblin.toent()->getcomponent <Collider>().hasrigidbody = true;
}
