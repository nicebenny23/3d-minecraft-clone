#pragma once
#include "player.h"
#include "../game/rigidbody.h"
#include "inventory_ui.h"
#include "playerattack.h"
#include "playerplace.h"
#include "cameracomp.h"
#include "playermovment.h"
#include "playermodification.h"
#include "playereat.h"
#include "playercamcontrols.h"

#include "crosshair.h"
#include "../renderer/ModelMesh.h"
#include "../game/close.h"

void initplayer(ecs::obj& player) {
	using namespace player;
	float playerfric = 5;
	player.world().insert_resource<PlayerResource>(player);
	player.add_component<PlayerTag>();
	player.add_component<core::LocalTransform>(v3::Point3(0, 2, 0));
	player.get_component<core::LocalTransform>().transform.scale = unit_scale / 1.2f;

	timing::Clock& clock = player.world().get_resource<timing::GameClock>().game_clock;
	player.add_component<player::CloseMenuComponent>(ecs::spawn(player.world(), player::make_close_menu));
	collision::DynamicColliderRecipe().apply(player);
	player.apply_recipe(physics::Spawner{ .restitution = .6,.gravity = v3::Vec3(0,-18,0) });
	ecs::obj eater = ecs::spawn(player.world(), ui::ImageSpawner(geo::Box2d::origin_centered(v2::Vec2(.4f, .4f)), 1));
	player.add_component<player::player_eat_behavior>(clock, eater);

	player.apply_recipe(player::player_health_spawner);

	player.add_component< playerbreak>();
	player.add_component< player_place>();
	ecs::obj spawned = ecs::spawn(player.world(), wireframe_recipe);
	player.add_component<PlayerCursor>(spawned);
	player.add_component<PlayerAttack>(ecs::spawn(player.world(), renderer::ParticleEmmitterRecipe<PlayerAttackParticleSpawner>{.max_lifetime = 2.0f}), clock);
	player.add_component<renderer::CameraComponent>();
	ecs::spawn(player.world(), CameraSpawner()).add_component<renderer::CameraDirectFollower>(player);
	player.add_component<PlayerMovment>(clock);
	player.add_component<CameraController>();
}

void player::player_plugin(core::App& app) {

	using namespace player;

	app.insert_plugin(player_place_plugin);
	ecs::spawn(app.Ecs, initplayer);
	app.insert_plugin(player_modification_plugin);
	app.insert_plugin(player::crosshair_plugin);
	app.insert_plugin(player::player_inventory_plugin);
	app.emplace_system<PlayerAttacker>();
	app.emplace_system<PlayerMovementSys>();
	app.emplace_system<PlayerHealthUi>();
	app.insert_plugin(model_plugin);
	app.insert_plugin(player::close_menu_plugin);
	app.emplace_system<CameraControlSystem>();
	app.emplace_system<player::PlayerEater>();
	app.emplace_system<CameraFollowerSystem>();
}
