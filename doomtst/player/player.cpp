#include "player.h"
#include "../game/rigidbody.h"
#include "playerinventory.h"
#include "playerattack.h"
#include "playerplace.h"
#include "playermodification.h"
#include "playereat.h"
using namespace player;
entityname::entityref player::goblin;

void player::initplayer()
{
	goblin = entityname::createentity(Vector3(0,300,0), "");
	goblin.toent()->addcomponent<estate>(10,true);
	goblin->addcomponent<playereat>();
	goblin.toent()->addcomponent<playerhealth>();
	goblin.toent()->addcomponent<inventory>();
	goblin.toent()->addtag("player");
	goblin->addcomponent<playerclimb>();
		goblin.toent()->addcomponent<Collider>(zerov, unitv / 2.3f, true);
	goblin.toent()->addcomponent<playermovement>();
	goblin.toent()->addcomponent< playerbreak>();
	goblin.toent()->addcomponent< playerplace>();
	float playerfric = 5;
	goblin.toent()->transform.scale = unitscale;
	goblin.toent()->addcomponent<rigidbody>(playerfric);
	goblin.toent()->addcomponent<playerattackcomp>();
	goblin.toent()->getcomponent <Collider>().hasrigidbody = true;
}
