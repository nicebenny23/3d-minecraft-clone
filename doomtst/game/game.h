#include "../game/camera.h"
#include "../renderer/ModelMesh.h"
#include <iostream>
#include <cmath>
#include "../renderer/renderer.h"
#include "../renderer/Window.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../util/userinput.h"
#include "../world/chunk.h"
#include "../game/objecthelper.h"
#include "../world/grid.h"
#include "../renderer/blockrender.h"
#include "../world/managegrid.h"
#include "../util/time.h"
#include "../game/collision.h"
#include "../util/fileloader.h"
#include "../player/player.h"
#include "../game/navigation.h"
#include "../renderer/grandrenderer.h"
#include "../world/noise.h"
#include "../world/tick.h"
#include "../block/liquid.h"
#include "../debugger/console.h"
#include "../items/menu.h"
#include "Core.h"
#include "rigidbody.h"
#include "../player/playermovment.h"
#include "../game/System.h"
#include "../game/entity.h"
#include "../entities/entityspawner.h"
#pragma once 

void endframe() {
	CtxName::ctx.Inp->endupdate();
	updateltick();
	CtxName::ctx.Window->SwapBuffers();
	glfwPollEvents();
	CtxName::ctx.Ren->Clear();
}
void startframe() {


	CtxName::ctx.Time->calcfps();
	tick::trytick();
	managemenus();

}

void updateworld() {
	collision::update();
	CtxName::ctx.OC->run_systems();
	if (isnan(player::goblin.get_component<rigidbody>().velocity.x)) {
		int l = 2;
	}
	gridutil::gridupdate();
	if (isnan(player::goblin.get_component<rigidbody>().velocity.x)) {
		int l = 2;
	}
}
void update() {

	startframe();

	updateworld();
	rendergame();
	endframe();
}
void init() {

	Core::game.ConnectToContext();

	Core::game.createWindow();

	Core::game.InitRenderer();
	initrandom();
	Core::game.CreateWorld();
	Core::game.InitOC();
	aabb::initCollider();
	ui::createuilist();
	inittextarray();

	create_decal_material();

	blockrender::initblockrendering();

	ui::createuielement<uibox>("images\\crosshair.png", "CrosshairTexture", v2::unitv / 32, v2::zerov, -3);

	Core::game.CreateGrid();
	player::initplayer();
	gridutil::computeallcover();
	gridutil::redolighting();
	CtxName::ctx.Inp->endupdate();

	glfwSwapInterval(0);
}

void endgame() {

	guirender::destroygui();
	glfwTerminate();

}
void rungame() {
	init();

	CtxName::ctx.OC->emplace_system<RigidbodySystem>();
	CtxName::ctx.OC->emplace_system<spawn_mobs>();
	CtxName::ctx.OC->emplace_system<PlayerMovementSys>();
	// createslime(zerov+Vec3(0,10,0), false);
	float lastupdate = 0;
	{

	}
	while (!CtxName::ctx.Window->shouldClose()) {

		update();
	}
	endgame();
	return;
}

