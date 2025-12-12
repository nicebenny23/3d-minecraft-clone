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
ecs::obj player::goblin;

void player::initplayer()
{

	float playerfric = 5;
	goblin = CtxName::ctx.OC->spawn_empty();
	goblin.add_component<ecs::transform_comp>(v3::Vec3(0,2,0));
	goblin.add_component<estate>(10, true);
	goblin.get_component<ecs::transform_comp>().transform.scale = unitv / 2;

	goblin.add_component<inventory>();
	goblin.add_component<Collider>(zerov, unitv / 2.3f, true, false);
	goblin.add_component<rigidbody>(playerfric);

	goblin.add_component<playereat>();
	goblin.add_component<playerhealth>();

	goblin.add_component<playertpcomp>();
	goblin.add_component<playerclimb>();


	goblin.add_component< playerbreak>();
	goblin.add_component< playerplace>();

	goblin.add_component<player_tag>();

	goblin.add_component<playerattackcomp>();
	goblin.add_component<playerdaggercomp>();
//	goblin.add_component<ecs::StaticComponent>();

	goblin.add_component<CameraComp>();
	goblin.add_component<playermovement>();
	goblin.add_component<playercamcontrols>();
}
