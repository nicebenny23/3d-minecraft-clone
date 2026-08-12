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
	struct BoxDisplay :ecs::component {
		stn::array<ecs::Constrained<ui::Image>> list;
	};
	struct PlayerHealth : ecs::component {
		PlayerHealth(ecs::Constrained<ui::Image> image,ecs::Constrained<BoxDisplay> water,ecs::Constrained<BoxDisplay> health):health_damage_box(image),water_boxes(water),health_boxes(health){

		}
		ecs::Constrained<ui::Image> health_damage_box;

		ecs::Constrained<BoxDisplay> water_boxes;
		ecs::Constrained<BoxDisplay> health_boxes;
	};

	struct DisplayedRecipe {
		void apply(ecs::obj& object) const {
			object.add_component< ui::Image>(colors::White,object.world().get_resource<Renderer>().gen_renderable("Ui"));
			object.world().add_component<BoxDisplay>(object.get_component<ecs::Child>().parent()).list.emplace(object);
		}
	};

	struct PlayerHealthUi :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View< PlayerHealth, Health::EntityHealth,Health::Drownable> healthboxes(world);
			for (auto&& [health_ui, health,drown] : healthboxes) {
				if (health.damage_delay_timer.is_active()) {
					health_ui.health_damage_box.get_component<ui::UiEnabled>().enable();
				}
				else {
					health_ui.health_damage_box.get_component<ui::UiEnabled>().disable();
				}
				for (int i = 0; i < health.current_health; i++) {
				}
				for (int i = 0; i < health.max_health; i++) {
					auto at = health_ui.health_boxes.get<BoxDisplay>().list[i];

					if (i<health.current_health) {
						at.get<ui::Image>().set_image("images\\health.png");
					}
					else{
						at.get<ui::Image>().set_image("images\\NoHealth.png");
					}
				}
				for (int i = 0; i < drown.max_bars; i++) {
					auto at = health_ui.water_boxes.get<BoxDisplay>().list[i];
					at.get<ui::Image>().set_image("images\\water_bubble.png");
					at.get_component<ui::UiEnabled>().set_enabled(drown.bars!= drown.max_bars&& i<drown.bars);
				}
				if (health.current_health==0) {
					world.write_command(core::CloseGameCommand());
				}
			}


		}
	};
	
	inline void player_health_spawner(ecs::obj& player) { 

		player.apply_recipe(Health::HealthSpawner(10));
		player.add_component<Health::Drownable>(5);

		ecs::obj spawn = ecs::spawn(player.world(), ui::ImageSpawner(renderer::TexturePath("images\\default.png"), geo::unit_box_2d, 0, colors::Red.with_opacity(.2f)));
		ui::UiSpawner health_ui(geo::Box2d(v2::Vec2(.1f, -.35f) / 2, v2::Vec2(.1f, .015f)), 12);
		ui::UiSpawner bx= ui::UiSpawner(geo::Box2d::origin_centered(v2::Vec2(1.2f, 1) * .7f), 1);
		ui::TableBounds health_bounds(player.get_component<Health::EntityHealth>().max_health, 1);
		ecs::obj health_boxes = ecs::spawn(player.world(), ui::UiTableRecipe< DisplayedRecipe>(health_ui, DisplayedRecipe(), health_bounds, bx));
		size_t bnds = player.get_component<Health::Drownable>().bars;
		ui::UiSpawner water_ui(geo::Box2d(v2::Vec2(.3f, -.35f) / 2, v2::Vec2(.01f*bnds, .01f)), 12);
		ui::TableBounds water_bounds(player.get_component<Health::Drownable>().bars, 1);
		ecs::obj water_boxes= ecs::spawn(player.world(), ui::UiTableRecipe< DisplayedRecipe>(water_ui, DisplayedRecipe(), water_bounds, bx));

		PlayerHealth& h=player.add_component<PlayerHealth>(spawn,water_boxes,health_boxes);
	
	}

	inline void player_health_plugin(core::App& app) {
		app.emplace_system<PlayerHealthUi>();

	}
}