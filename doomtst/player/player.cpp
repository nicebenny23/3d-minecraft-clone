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
#include "../renderer/ModelMesh.h"
ecs::obj player::goblin;

void player::initplayer(ecs::obj& player) {
	goblin = player;
	using namespace player;
	float playerfric = 5;
	player.add_component<player_tag>();
	player.world().insert_resource<player_resource>(player);

	player.add_component<core::LocalTransform>(v3::Point3(0, 2, 0));
	player.apply_recipe(Health::HealthSpawner(10));
	player.get_component<core::LocalTransform>().transform.scale = unit_scale / 1.2f;
	core::game.insert_plugin(PlayerModificationPlugin());
	core::game.insert_plugin(PlayerPlacePlugin());
	core::game.insert_plugin(player::CrosshairPlugin());
	core::game.insert_plugin(PlayerInventoryPlugin());
	core::game.emplace_system<PlayerAttacker>();
	core::game.emplace_system<PlayerMovementSys>();
	core::game.emplace_system<PlayerHealthUi>();
	core::game.insert_plugin(ModelPlugin());

	aabb::DynamicColliderRecipe().apply(player);
	player.apply_recipe(physics::Spawner{.restitution=.6});
	core::game.emplace_system<player::PlayerEater>();
	ecs::obj eater = ecs::spawn(player.world(),ui::UiImageSpawner(geo::Box2d::origin_centered(v2::Vec2(.4f, .4f)),1));
	player.add_component<player::player_eat_behavior>(player.world().get_resource<timing::WorldClock>(),eater);
	player.apply_recipe(player::player_health_spawner);  

	player.add_component<playertpcomp>();
	player.add_component<playerclimb>();
	player.add_component<Health::FallDamageRecipient>(.19f);

	player.add_component< playerbreak>();
	player.add_component< player_place>();
	ecs::obj spawned = ecs::spawn(player.world(),wireframe_recipe);
	player.add_component<PlayerCursor>(spawned);
	player.add_component<PlayerAttack>(ecs::spawn(player.world(), renderer::ParticleEmmitterRecipe<PlayerAttackParticleSpawner>{.max_lifetime=2.0f}),player.world().get_resource<timing::WorldClock>());
	player.add_component<playerdaggercomp>();
	player.add_component<renderer::CameraComponent>();
	ecs::spawn(player.world(), CameraSpawner()).add_component<renderer::CameraDirectFollower>(player);
	core::game.emplace_system<CameraFollowerSystem>();
	player.add_component<playermovement>();
	player.add_component<playercamcontrols>();
}
