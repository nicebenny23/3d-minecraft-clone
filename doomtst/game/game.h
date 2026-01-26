#include "../renderer/renderer.h"
#include "../renderer/Window.h"

#include "../util/userinput.h"
#include "../items/menu.h"
#include "../renderer/blockrender.h"
#include "../game/collision.h"
#include "../player/player.h"
#include "../world/tick.h"
#include "../block/liquid.h"
#include "Core.h"
#include "rigidbody.h"
#include "../player/playermovment.h"

#include "../entities/entityspawner.h"
#include "../renderer/decal.h"

#pragma once 

void endframe() {
	CtxName::ctx.Inp->endupdate();
	updateltick();
	CtxName::ctx.Window->SwapBuffers();
	glfwPollEvents();
	CtxName::ctx.Ren->Clear();
}
void startframe() {


	Core::game.ensure_resource<timename::TimeManager>().calcfps();
	tick::trytick();


}

void updateworld() {
	collision::update();
	CtxName::ctx.Ecs->run_systems();


}
void update() {

	startframe();

	updateworld();
	endframe();
}
struct TimePlugin :Core::Plugin {
	void build(Core::App& App) {
		App.emplace_resource<timename::TimeManager>();
	}
};
struct RenderPlugin :Core::Plugin {
	void build(Core::App& App) {

	App.Ecs.emplace_asset_loader<assets::SelfDescriptorLoader<renderer::render_phase>>();
	App.Ecs.emplace_asset_loader<renderer::TextureLoader>();
	App.Ecs.emplace_asset_loader<renderer::TextureArrayLoader>();
	App.Ecs.emplace_asset_loader<renderer::shader_loader>();
	App.ensure_resource<renderer::Renderer>();
	App.Ecs.emplace_system<renderer::render_all>();
	}
};

void init() {
	Core::game.ConnectToContext();

	Core::game.InitOC();

	Core::game.createWindow();
	Core::game.insert_plugin<RenderPlugin>();
	Core::game.insert_plugin<ui::UiImagePlugin>();

	Core::game.insert_plugin<ui::MenuPlugin>();
	Core::game.insert_plugin<items::register_core_items>();
	Core::game.insert_plugin<items::ItemUiPlugin>();
	Core::game.insert_plugin<ui::UiTextPlugin>();
	

	Core::game.InitRenderer();
	random::initrandom();
	Core::game.CreateWorld();

	aabb::initCollider();
	

	//ui::createuielement<ui_image_component>("images\\crosshair.png", "CrosshairTexture", v2::unitv / 32, v2::zerov, -3);

	Core::game.CreateGrid();
	player::initplayer();
	
	CtxName::ctx.Inp->endupdate();

	glfwSwapInterval(0);
}

void endgame() {

	guirender::destroygui();
	glfwTerminate();

}
void rungame() {
	init();

	CtxName::ctx.Ecs->emplace_system<RigidbodySystem>();
	CtxName::ctx.Ecs->emplace_system<spawn_mobs>();
	CtxName::ctx.Ecs->emplace_system<PlayerMovementSys>();
	Core::game.insert_plugin<blockrender::BlockRenderPlugin>();
	Core::game.insert_plugin<decal_plugin>();
	float lastupdate = 0;
	while (!CtxName::ctx.Window->shouldClose()) {
		update();
	}
	endgame();
	return;
}

