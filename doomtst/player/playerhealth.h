#pragma once
#include "../util/dynamicarray.h"
#include "../renderer/uibox.h"
#include "../game/ecs/game_object.h"
#include <conio.h>
#include "../items/block_definitions.h"
#include "playerinventory.h"
#include "../util/cached.h"
#include "../game/health.h"

#pragma once
#include "../util/dynamicarray.h"
#include "../renderer/uibox.h"
#include "../game/ecs/game_object.h"
#include <conio.h>
#include "../items/block_definitions.h"
#include "playerinventory.h"
#include "../util/cached.h"
#include "../game/health.h"
namespace player {
	struct PlayerHealth : ecs::component {
		PlayerHealth(ecs::Constrained<ui::UiImage> image):health_damage_box(image){

		}
		ecs::Constrained<ui::UiImage> health_damage_box;
		array<ecs::Constrained<ui::UiImage>> health_boxes;
	};
	struct HealthDisplayedRecipe {
		void apply(ecs::obj& object) const {
			v2::UVec2 index = object.get_component<ui::TableEntry>().entry;
			ecs::obj parent = ecs::parent(object).unwrap();
			object.add_component< ui::UiImage>(colors::White,object.world().get_resource<Renderer>().gen_renderable("Ui"));
			parent.get_component<PlayerHealth>().health_boxes.push(object);
		}
	};
	struct PlayerHealthUi :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View< PlayerHealth, Health::EntityHealth> healthboxes(world);
			for (auto&& [health_ui, health] : healthboxes) {
				if (health.damage_delay_timer.is_active()) {
					health_ui.health_damage_box.get_component<ui::UiEnabled>().enable();
				}
				else {
					health_ui.health_damage_box.get_component<ui::UiEnabled>().disable();
				}
				for (int i = 0; i < health.current_health; i++) {
					health_ui.health_boxes[i].get<ui::UiImage>().set_image("images\\health.png");

					health_ui.health_boxes[i].get_component<ui::UiEnabled>().enable();
				}
				for (int i = health.current_health; i < health.max_health; i++) {
					health_ui.health_boxes[i].get<ui::UiImage>().set_image("images\\NoHealth.png");

					health_ui.health_boxes[i].get_component<ui::UiEnabled>().enable();
				}
				if (health.current_health==0) {
					world.write_command(Core::CloseGameCommand());
				}
			}


		}
	};
	inline void player_health_spawner(ecs::obj& player) { 
		ecs::obj spawn = ecs::spawn(player.world(), ui::UiImageSpawner(renderer::TexturePath("images\\default.png"), geo::unit_box_2d, 0, colors::Red.with_opacity(.2f)));

		player.add_component<PlayerHealth>(spawn);
		ui::UiSpawner ui(geo::Box2d(v2::Vec2(.1f, -.35f)/2, v2::Vec2(.1f, .015f)), 12);
		ui::TableBounds bounds(player.get_component<Health::EntityHealth>().max_health, 1);
		player.apply_recipe(ui::UiTableRecipe< HealthDisplayedRecipe >(ui, HealthDisplayedRecipe(), bounds, ui::UiSpawner(geo::Box2d::origin_centered(v2::unitv*.7f), 1)));
	}
	inline void player_health_plugin(Core::App& app) {
		app.emplace_system<PlayerHealthUi>();
	}
}