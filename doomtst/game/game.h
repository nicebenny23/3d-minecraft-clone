#include "../renderer/renderer.h"
#include "../renderer/Window.h"
#include "../util/userinput.h"
#include "../items/menu.h"
#include "../renderer/blockrender.h"
#include "../game/collision.h"
#include "../player/player.h"
#include "../block/liquid.h"
#include "Core.h"
#include "rigidbody.h"
#include "../player/playermovment.h"
#include "../entities/entityspawner.h"

#include "../renderer/guirender.h"
#include "../player/inventory_ui.h"
#include "particles.h"
#include "../world/terrain.h"
#include "../util/unique.h"
#pragma once 

void endframe() {
	Core::game.Ecs.get_resource<renderer::Window>().swap_buffers();
	Core::game.Ecs.get_resource<renderer::Renderer>().clear();
}

void update() {
	Core::game.Ecs.run_systems();
	endframe();
}





struct MinecraftPlugin {
	void operator()(Core::App& app) {

		app.insert_plugin(Core::GamePlugin());

		app.insert_plugin(userinput::user_input_plugin);
		app.emplace_resource<userinput::InputManager>();
		renderer::Window& window = app.Ecs.get_resource<renderer::Window>();
		window.set_icon("images\\crystaloreenhanced.png");
		window.set_name("benny render 3d");
		app.insert_plugin(timing::TimePlugin());
		app.insert_plugin(renderer::RendererPlugin());
		app.insert_plugin(ui::MenuPlugin());
		app.insert_plugin(items::ItemUiPlugin());
		app.insert_plugin(items::register_core_items);
		app.insert_plugin(collision::CollsionPlugin());
		app.emplace_system<userinput::InputPollingSystem>();
		app.emplace_resource<grid::World>(0);
		random::initilize_random();
		app.insert_plugin(renderer::ParticlePlugin());
		ecs::obj player= ecs::spawn(app.Ecs, player::initplayer);
		grid::Grid& world= app.emplace_resource<grid::Grid>(3  , player,stn::box<world::TerrainGenerator>(stn::construct_derived<world::DefaultTerrainGenerator>(),app.Ecs.get_resource<blocks::BlockRegistry>()));
		blocks::BlockRegistry& registry = app.Ecs.get_resource<blocks::BlockRegistry>();
		math::Transform& transform = player.get_component<core::LocalTransform>().transform;
		transform.position = voxtra::move_left_until_air(transform.unrotated_box(), world, registry).center;
		glfwSwapInterval(0);
		app.insert_plugin(timing::TimePlugin());
		app.insert_plugin(physics::PhysicsPlugin());
		app.insert_plugin(game::MobSpawnerPlugin);
		app.insert_plugin(Health::EntityHealthPlugin());
		app.insert_plugin(blockrender::BlockRenderPlugin());
		app.insert_plugin(player::PlayerInventoryPlugin());
		app.insert_plugin(guirender::ConsolePlugin());
		app.insert_plugin(renderer::ModelPlugin());

	}


};

void rungame() {
	Core::game.insert_plugin(MinecraftPlugin());
	while (!Core::game.Ecs.get_resource<Core::GameState>().should_close) {
		update();
	}
	guirender::destroygui();
	glfwTerminate();
}

