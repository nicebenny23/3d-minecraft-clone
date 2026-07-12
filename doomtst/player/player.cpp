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
#include "player_fertilize.h"
#include "../renderer/ModelMesh.h"
#include "../game/close.h"

void player::initplayer(ecs::obj& player) {
	using namespace player;
	float playerfric = 5;
	player.world().insert_resource<PlayerResource>(player);
	player.add_component<PlayerTag>();
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
	core::game.insert_plugin(player::CloseMenuPlugin);
	core::game.emplace_system<CameraControlSystem>();
	core::game.emplace_system<player::PlayerEater>();
	core::game.emplace_system<CameraFollowerSystem>();
	core::game.emplace_system<player::FertilizationSystem>();
	timing::WorldClock& clock = player.world().get_resource<timing::WorldClock>();
	ecs::obj fertilizer=ecs::spawn(player.world(), renderer::ParticleEmmitterRecipe<PlayerFertilizeParticleSpawner>{.max_lifetime = 2.0f});
	player.add_component<PlayerFertilizer>(fertilizer);
		player.add_component<player::CloseMenuComponent>(ecs::spawn(player.world(),player::make_close_menu));
	aabb::DynamicColliderRecipe().apply(player);
	player.apply_recipe(physics::Spawner{.restitution=.6});
	ecs::obj eater = ecs::spawn(player.world(),ui::ImageSpawner(geo::Box2d::origin_centered(v2::Vec2(.4f, .4f)),1));
	player.add_component<player::player_eat_behavior>(clock,eater);
	player.apply_recipe(player::player_health_spawner);  

	player.add_component<playertpcomp>();
	player.add_component<playerclimb>();

	player.add_component< playerbreak>();
	player.add_component< player_place>();
	ecs::obj spawned = ecs::spawn(player.world(),wireframe_recipe);
	player.add_component<PlayerCursor>(spawned);
	player.add_component<PlayerAttack>(ecs::spawn(player.world(), renderer::ParticleEmmitterRecipe<PlayerAttackParticleSpawner>{.max_lifetime=2.0f}), clock);
	player.add_component<playerdaggercomp>();
	player.add_component<renderer::CameraComponent>();
	ecs::spawn(player.world(), CameraSpawner()).add_component<renderer::CameraDirectFollower>(player);
	player.add_component<playermovement>(clock);
	player.add_component<CameraController>();
}
