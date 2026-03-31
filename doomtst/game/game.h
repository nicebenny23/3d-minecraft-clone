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
#include "../renderer/decal.h"
#include "../renderer/guirender.h"
#include "../player/inventory_ui.h"
#pragma once 

void endframe() {
	Core::game.Ecs.get_resource<window::Window>().SwapBuffers();
	Core::game.Ecs.get_resource<renderer::Renderer>().clear();
}

void update() {
	Core::game.Ecs.run_systems();
	endframe();
}
struct MinecraftPlugin :Core::Plugin {
	void build(Core::App& app) {

		app.insert_plugin<Core::GamePlugin>();
		app.createWindow();
		app.insert_plugin< timename::TimePlugin>();
		app.insert_plugin<renderer::RendererPlugin>();
		app.insert_plugin<ui::MenuPlugin>();
		app.insert_plugin<items::ItemUiPlugin>();
		app.insert_plugin<items::register_core_items>();
		app.insert_plugin<collision::CollsionPlugin>();
		app.emplace_system<userinput::InputPollingSystem>();
		app.emplace_resource<grid::world>(0);
		random::initilize_random();

		ecs::obj player= ecs::spawn(app.Ecs, player::initplayer);
		app.emplace_resource<grid::Grid>(3, player);

		glfwSwapInterval(0);
		app.insert_plugin<timename::TimePlugin>();
		app.insert_plugin<physics::PhysicsPlugin>();
		app.insert_plugin<MobSpawnerPlugin>();
		app.insert_plugin<Health::EntityHealthPlugin>();
		app.insert_plugin<blockrender::BlockRenderPlugin>();
		app.insert_plugin<player::PlayerInventoryPlugin>();
		app.insert_plugin<decals::decal_plugin>();
		app.insert_plugin<guirender::ConsolePlugin>();
		app.insert_plugin<renderer::ModelPlugin>();

	}


};

void rungame() {
	Core::game.insert_plugin<MinecraftPlugin>();
	while (!Core::game.Ecs.get_resource<Core::GameState>().should_close) {
		update();
	}
	guirender::destroygui();
	glfwTerminate();
}

