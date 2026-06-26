#include "../renderer/uibox.h"
#include "../items/menu.h"
#include "player.h"
#include "../game/ecs/game_object.h"
#include "../game/Core.h"
#include "playerattack.h"
#pragma once
namespace player {

	struct crosshair_component:ecs::component {
		crosshair_component(ecs::Constrained<ui::UiImage> crosshair_handle) :crosshair(crosshair_handle) {

		}
		ecs::Constrained<ui::UiImage> crosshair;
	};
	struct CrosshairSystem:ecs::System{
		virtual void run(ecs::Ecs& world) {
			bool enabled = world.get_resource<ui::MenuState>().no_menu_open();
			auto view=ecs::View<crosshair_component,player::PlayerAttack,player::PlayerCursor>(world);
			for (auto[crosshair,attacker,cursor]:view ) {
				if (attacker.last_attack.is_inactive()&& cursor.hit_entity()) {
					crosshair.crosshair.get_component<ui::UiImage>().set_image(renderer::TexturePath("images\\active_crosshair.png"));
				}
				else {
					crosshair.crosshair.get_component<ui::UiImage>().set_image(renderer::TexturePath("images\\crosshair.png"));
				}
				crosshair.crosshair.get_component<ui::UiEnabled>().set_enabled(enabled);
			}
		}
	};
	struct CrosshairPlugin {
		void operator()(core::App& app) {
			app.emplace_system< CrosshairSystem>();
			ecs::Constrained<ui::UiImage> crosshair_handle(ecs::spawn(app.Ecs, ui::ImageSpawner(geo::Box2d::origin_centered(v2::unitv / 32), -3)));
			player_for(app.Ecs).add_component < crosshair_component>(crosshair_handle);

		}
	};







}