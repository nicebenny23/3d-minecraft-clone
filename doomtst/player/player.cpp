#pragma once
#include "player.h"
#include "../game/rigidbody.h"
#include "playerinventory.h"
#include "playerattack.h"
#include "playerplace.h"
#include "playerclimb.h"
#include "cameracomp.h"
#include "playermovment.h"
#include "playermodification.h"
#include "playereat.h"
#include "playercamcontrols.h"
#include "daggerthrow.h"
#include "playertpsword.h"
using namespace player;
gameobject::obj player::goblin;

void player::initplayer()
{

	float playerfric = 5;
	goblin = CtxName::ctx.OC->CreateEntity(Vec3(0, 2, 0));

	goblin.addcomponent<estate>(10, true);
	goblin.transform().scale = unitv / 2;

	goblin.addcomponent<inventory>();
	goblin.addcomponent<Collider>(zerov, unitv / 2.3f, true, false);
	goblin.addcomponent<rigidbody>(playerfric);

	goblin.addcomponent<playereat>();
	goblin.addcomponent<playerhealth>();

	goblin.addcomponent<playertpcomp>();
	goblin.addcomponent<playerclimb>();


	goblin.addcomponent< playerbreak>();
	goblin.addcomponent< playerplace>();

	goblin.addcomponent<player_tag>();

	goblin.addcomponent<playerattackcomp>();
	goblin.addcomponent<playerdaggercomp>();
	goblin.addcomponent<gameobject::StaticComponent>();

	goblin.addcomponent<CameraComp>();
	goblin.addcomponent<playermovement>();
	goblin.addcomponent<playercamcontrols>();
}
