#include "player.h"
#include "../game/rigidbody.h"
#include "playerinventory.h"
#include "playerattack.h"
#include "playerplace.h"
#include "playermodification.h"
#include "playereat.h"
#include "../renderer/Window.h"
#include "daggerthrow.h"
using namespace player;
entityname::entityref player::goblin;

void player::initplayer()
{
	goblin = entityname::createentity(Vector3(0,0,10), "");
	goblin->canbedestroyed = false;
	goblin.toent()->addcomponent<estate>(10,true);
	goblin->addcomponent<playereat>();
	goblin.toent()->addcomponent<playerhealth>();
	goblin.toent()->addcomponent<inventory>();
	goblin.toent()->addtag("player");
	goblin->addcomponent<playertpcomp>();
	goblin->addcomponent<playerclimb>();
		goblin.toent()->addcomponent<Collider>(zerov, unitv / 2.3f, true);
	goblin.toent()->addcomponent<playermovement>();
	goblin.toent()->addcomponent< playerbreak>();
	goblin.toent()->addcomponent< playerplace>();
	float playerfric = 5;
	goblin.toent()->transform.scale = unitscale;
	goblin.toent()->addcomponent<rigidbody>(playerfric);
	goblin.toent()->addcomponent<playerattackcomp>();
	goblin.toent()->addcomponent<playerdaggercomp>();
}

void player::calculateyawandpitch()
{
	
		if (!ismenuopen())
		{
			window::setcursor(false);

			float xoffset = -userinput::mouseposdt.x;
			float yoffset = userinput::mouseposdt.y;

			float sensitivity = 0.3;

			if (abs(xoffset) > 180)
			{
				xoffset = 0;
			}  if (abs(yoffset) > 180)
			{
				yoffset = 0;
			}
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			goblin->transform.yaw += xoffset;
			goblin->transform.pitch += yoffset;

			if (goblin->transform.pitch > 89.0f)
				goblin->transform.pitch = 89.0f;
			if (goblin->transform.pitch < -89.0f)
				goblin->transform.pitch = -89.0f;

		}
		else
		{
			window::setcursor(true);
		}
	
}
