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
#pragma once 

void endframe() {
	Core::game.Ecs.get_resource<userinput::InputManager>().endupdate();
	updateltick();
	Core::game.Ecs.get_resource<window::Window>().SwapBuffers();
	glfwPollEvents();
	Core::game.Ecs.get_resource<renderer::Renderer>().Clear();
}

void update() {
	Core::game.Ecs.run_systems();
	endframe();
}

void init() {
	Core::game.ConnectToContext();
	Core::game.InitOC();
	Core::game.createWindow();
	Core::game.insert_plugin<renderer::RendererPlugin>();
	Core::game.insert_plugin<ui::UiImagePlugin>();
	Core::game.insert_plugin<ui::MenuPlugin>();
	Core::game.insert_plugin<items::register_core_items>();
	Core::game.insert_plugin<items::ItemUiPlugin>();
	Core::game.insert_plugin<ui::UiTextPlugin>();
	Core::game.insert_plugin<collision::CollsionPlugin>();
	random::initrandom();
	Core::game.CreateWorld();
	//ui::createuielement<ui_image_component>("images\\crosshair.png", "CrosshairTexture", v2::unitv / 32, v2::zerov, -3);
	Core::game.emplace_resource<grid::Grid>(2);
	player::initplayer();
	glfwSwapInterval(0);
}

struct PlayerInventoryPlugin :Core::Plugin {
	void build(Core::App& world) {
		world.emplace_system<player::LoadHotbarSlots>();
	}
};
void rungame() {
	init();

	Core::game.insert_plugin<timename::TimePlugin>();
	Core::game.Ecs.emplace_system<RigidbodySystem>();
	Core::game.Ecs.emplace_system<spawn_mobs>();
	Core::game.Ecs.emplace_system<PlayerMovementSys>();
	Core::game.insert_plugin<blockrender::BlockRenderPlugin>();
	Core::game.insert_plugin<PlayerInventoryPlugin>();
	Core::game.insert_plugin<decals::decal_plugin>();
	Core::game.insert_plugin<guirender::ConsolePlugin>();
	while (!Core::game.Ecs.get_resource<Core::GameState>().should_close) {
		update();
	}
	guirender::destroygui();
	glfwTerminate();
}

