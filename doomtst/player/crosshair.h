#include "../renderer/uibox.h"
#include "../items/menu.h"
#include "player.h"
#include "../game/ecs/game_object.h"
#include "../game/Core.h"
#pragma once
namespace player {

	struct crosshair_component:ecs::component {
		crosshair_component(ecs::Constrained<ui::ui_image_component> crosshair_handle) :crosshair(crosshair_handle) {

		}
		ecs::Constrained<ui::ui_image_component> crosshair;
	};
	struct CrosshairSystem:ecs::System{
		virtual void run(ecs::Ecs& world) {
			bool enabled = world.get_resource<ui::MenuState>().no_menu_open();
			auto view=ecs::View<crosshair_component>(world);
			for (auto[crosshair]:view ) {
				crosshair.crosshair.get_component<ui::UiEnabled>().set_enabled(enabled);
			}
		}
	};
	struct CrosshairPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system< CrosshairSystem>();
			ecs::Constrained<ui::ui_image_component> crosshair_handle(ecs::spawn(app.Ecs, ui::ui_image_spawner(renderer::TexturePath("images\\crosshair.png"), geo::Box2d::origin_centered(v2::unitv / 32), -3)));
			player_for(app.Ecs).add_component < crosshair_component>(crosshair_handle);

		}
	};







}