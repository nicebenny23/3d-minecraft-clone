#include "../renderer/renderer.h"
#include "../renderer/Window.h"
#include "../util/userinput.h"
#include "../items/menu.h"
#include "../renderer/blockrender.h"
#include "../player/player.h"
#include "Core.h"
#include "rigidbody.h"
#include "../entities/entityspawner.h"
#include "../renderer/guirender.h"
#include "../player/inventory_ui.h"
#include "particles.h"
#include "../world/terrain.h"
#include "../util/unique.h"
#pragma once 

inline void minecraft_plugin(core::App& app) {
	random::init_random();
	app.insert_plugin(core::game_plugin);
	app.insert_plugin(userinput::user_input_plugin);
	renderer::Window& window = app.Ecs.get_resource<renderer::Window>();
	window.set_icon("images\\crystaloreenhanced.png");
	window.set_name("benny render 3d");
	app.insert_plugin(renderer::renderer_plugin);
	app.insert_plugin(ui::menu_plugin);
	app.insert_plugin(items::item_ui_plugin);
	app.insert_plugin(items::register_core_items);
	app.emplace_resource<grid::World>();
	app.emplace_system<timing::TickingSystem>();
	app.insert_plugin(renderer::particle_plugin);
	app.insert_plugin(physics::phycics_plugin);

	app.emplace_resource<timing::Ticks>(app.Ecs.get_resource<timing::WorldClock>());
	ecs::obj player = ecs::spawn(app.Ecs, player::initplayer);
	blocks::BlockRegistry& registry = app.Ecs.get_resource<blocks::BlockRegistry>();
	grid::Grid& world = app.emplace_resource<grid::Grid>(3, player, stn::box<world::TerrainGenerator>(stn::construct_derived<world::DefaultTerrainGenerator>(), registry));
	math::Transform& transform = player.get_component<core::LocalTransform>().transform;
	transform.position = voxtra::move_left_until_air(transform.unrotated_box(), world, registry).center;
	app.insert_plugin(game::MobSpawnerPlugin);
	app.insert_plugin(blocks::block_render_plugin);
	app.insert_plugin(player::player_inventory_plugin);
	app.insert_plugin(guirender::console_plugin);
	app.insert_plugin(renderer::model_plugin);
}
void rungame() {
	core::game.insert_plugin(minecraft_plugin);
	core::game.run();
	guirender::destroygui();

}

