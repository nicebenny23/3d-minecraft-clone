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

struct MinecraftPlugin {
	void operator()(core::App& app) {
		random::initilize_random();
		app.insert_plugin(core::GamePlugin());
		app.insert_plugin(userinput::user_input_plugin);
		renderer::Window& window = app.Ecs.get_resource<renderer::Window>();
		window.set_icon("images\\crystaloreenhanced.png");
		window.set_name("benny render 3d");
		app.insert_plugin(renderer::renderer_plugin);
		app.insert_plugin(ui::MenuPlugin());
		app.insert_plugin(items::ItemUiPlugin());
		app.insert_plugin(items::register_core_items);
		app.emplace_resource<grid::World>();
		app.insert_plugin(renderer::particle_plugin);
		ecs::obj player= ecs::spawn(app.Ecs, player::initplayer);
		grid::Grid& world= app.emplace_resource<grid::Grid>(3 , player,stn::box<world::TerrainGenerator>(stn::construct_derived<world::DefaultTerrainGenerator>(),app.Ecs.get_resource<blocks::BlockRegistry>()));
		blocks::BlockRegistry& registry = app.Ecs.get_resource<blocks::BlockRegistry>();
		math::Transform& transform = player.get_component<core::LocalTransform>().transform;
		transform.position = voxtra::move_left_until_air(transform.unrotated_box(), world, registry).center;
		app.insert_plugin(physics::phycics_plugin);
		app.insert_plugin(game::MobSpawnerPlugin);
		app.insert_plugin(blocks::block_render_plugin);
		app.insert_plugin(player::player_inventory_plugin);
		app.insert_plugin(guirender::ConsolePlugin());
		app.insert_plugin(renderer::ModelPlugin());
	}
};

void rungame() {
	core::game.insert_plugin(MinecraftPlugin());
	core::game.run();
	guirender::destroygui();
	
}

