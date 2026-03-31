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

	player.add_component<ecs::world_transform>(v3::Point3(0,2,0));
	player.apply_recipe(Health::HealthSpawner(10));
	player.get_component<ecs::world_transform>().transform.scale = unit_scale/1.02f;
	Core::game.insert_plugin<PlayerModificationPlugin>();
	Core::game.insert_plugin<PlayerPlacePlugin>();
	Core::game.insert_plugin<player::CrosshairPlugin>();
	Core::game.insert_plugin<PlayerInventoryPlugin>();
	Core::game.emplace_system<PlayerAttacker>();
	Core::game.emplace_system<PlayerMovementSys>();
	aabb::DynamicColliderRecipe().apply(player);
	player.apply_recipe(physics::Spawner);

	player.add_component<playereat>();
	player.add_component<playerhealth>();

	player.add_component<playertpcomp>();
	player.add_component<playerclimb>();


	player.add_component< playerbreak>();
	player.add_component< player_place>();
	player.add_component<PlayerCursor>();
	player.add_component<PlayerAttack>();
	player.add_component<Health::FallDamageRecipient>();
	player.add_component<playerdaggercomp>();
	player.add_component<renderer::CameraComponent>();
	
	player.world().insert_resource<renderer::camera_resource>(ecs::Constrained<renderer::CameraComponent>(player));
	player.add_component<playermovement>();
	player.add_component<playercamcontrols>();
}
