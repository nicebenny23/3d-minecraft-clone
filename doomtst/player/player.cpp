#pragma once
#include "player.h"
#include "../game/rigidbody.h"
#include "inventory_ui.h"
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
#include "crosshair.h"
ecs::obj player::goblin;

void player::initplayer(ecs::obj& player) {
	goblin = player;
	using namespace player;
	float playerfric = 5;
	player.add_component<player_tag>();
	player.world().insert_resource<player_resource>(player);

	player.add_component<ecs::transform_comp>(v3::Point3(0,2,0));
	player.add_component<estate>(10, true);
	player.get_component<ecs::transform_comp>().transform.scale = unit_scale/ 2;
	Core::game.insert_plugin<PlayerModificationPlugin>();
	Core::game.insert_plugin<PlayerPlacePlugin>();
	Core::game.insert_plugin<player::CrosshairPlugin>();
	Core::game.insert_plugin<PlayerInventoryPlugin>();
	aabb::DynamicColliderRecipe(geo::Box(v3::Point3(0, 0, 0), unit_scale / 2.3f)).apply(player);
	player.add_component<rigidbody>(playerfric);

	player.add_component<playereat>();
	player.add_component<playerhealth>();

	player.add_component<playertpcomp>();
	player.add_component<playerclimb>();


	player.add_component< playerbreak>();
	player.add_component< player_place>();
	player.add_component<PlayerCursor>();
	player.add_component<playerattackcomp>();
	player.add_component<playerdaggercomp>();
	player.add_component<CameraComp>();
	
	player.world().insert_resource<camera_resource>(ecs::Constrained<CameraComp>(player));
	player.add_component<playermovement>();
	player.add_component<playercamcontrols>();
}
