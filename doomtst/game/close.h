#include "../player/player.h"
#include "../renderer/ui.h"
#include "../items/menu.h"
#pragma once
namespace player {
	struct CloseButton :ecs::component {


	};
	struct CloseMenuComponent :ecs::component {
		CloseMenuComponent(ecs::Constrained<ui::MenuComponent> button) :button(button) {
		}
		ecs::Constrained<ui::MenuComponent> button;

	};
	inline void make_close_button(ecs::obj& object) {
		object.add_component<CloseButton>();
		ui::UiSpawner::with_default_priority(geo::Box2d(v2::zerov, v2::unitv/4.0)).apply(object);
		object.spawn_child_emplaced<ui::ImageSpawner>(renderer::TexturePath("images\\default.png"),geo::Box2d(v2::zerov, v2::unitv), 1);

		object.spawn_child_emplaced<ui::TextSpawner>(geo::Box2d(v2::zerov,v2::unitv*.2f), 2,colors::Black).get_component<ui::Text>().format("{}", "close");
	}
	inline void make_close_menu(ecs::obj& object) {
		ui::MenuRecipe().apply(object);
		object.spawn_child(make_close_button);

	}
	struct CloseGameOnCloseButton :ecs::System {
		CloseGameOnCloseButton(ecs::Ecs& world):reader(world.make_reader<ui::NoMenus>()) {
		}
		ecs::EventReader<ui::NoMenus> reader;
		void run(ecs::Ecs& world) {
			if (reader.read().nonempty()) {
				world.write_command(ui::menu_stack(player::player_for(world).get_component<CloseMenuComponent>().button));
			}
			ecs::View<ui::InteractionState, ecs::Has<CloseButton>> view(world);
			for (auto [interaction] : view) {
				if (interaction.left_clicked) {
					world.write_command(core::CloseGameCommand());
				}

			}

		}
	};
	inline bool in_game(const ecs::Ecs& world) {
		return !world.get_resource<ui::MenuState>().top().is_some();
		//.is_some_and(&ecs::obj::has_component<CloseMenuComponent>);
	}
	inline void CloseMenuPlugin(core::App& app) {
		app.emplace_system<CloseGameOnCloseButton>();
	}
}