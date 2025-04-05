#include "player.h"
#include "../game/rigidbody.h"
#include "playerinventory.h"
#include "playerattack.h"
#include "playerplace.h"
#include "playermodification.h"
#include "playereat.h"
#include "../renderer/Window.h"
#include "daggerthrow.h"
#include "playertpsword.h"
using namespace player;
entityname::entityref player::goblin;

void player::initplayer()
{

	float playerfric = 5;
	goblin = entityname::createentity(Vector3(0,00,0), "");
	goblin->canbedestroyed = false;
	goblin.toent()->addcomponent<estate>(10,true);
	goblin->addtag("player");
	goblin->transform.scale = unitv / 2;
	goblin->addcomponent<inventory>();
	goblin->addcomponent<Collider>(zerov, unitv / 2.3f, true, false);
	goblin->addcomponent<rigidbody>(playerfric);

	goblin->addcomponent<playereat>();
	goblin->addcomponent<playerhealth>();
	
	goblin->addcomponent<playertpcomp>();
	goblin->addcomponent<playerclimb>();
	
	
	goblin->addcomponent< playerbreak>();
	goblin->addcomponent< playerplace>();
	

	goblin->addcomponent<playerattackcomp>();
	goblin->addcomponent<playerdaggercomp>();
	

	goblin->addcomponent<playercam>();
	goblin->addcomponent<playermovement>();
	}

void player::calculateyawandpitch()
{
	
		if (!ismenuopen())
		{
			window::disableCursor();

			float xoffset = userinput::mouseposdt.x;
			float yoffset = -userinput::mouseposdt.y;

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

			goblin->transform.pitch=clamp(goblin->transform.pitch, -89.99, 89.99);

		}
		else
		{
			window::enableCursor();
		}
	
}
