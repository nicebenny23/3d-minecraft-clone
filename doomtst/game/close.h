#include "../player/player.h"
#include "../renderer/ui.h"
#include "../items/menu.h"
#pragma once
struct CloseButton:ecs::component{


};
struct ClosingMenu:ecs::component {
	

};
struct CloseMenuComponent:ecs::component {
	ecs::Constrained<ui::MenuComponent> button;

};
inline void make_close_button(ecs::obj& object) {
	object.add_component<CloseButton>();
	ui::UiSpawner::with_default_priority(geo::Box2d(v2::zerov, v2::unitv / 3)).apply(object);
	object.spawn_child<ui::UiTextSpawner>(geo::unit_box_2d,3).get_component<ui::Text>().format("{}","close");
}
inline void make_close_menu(ecs::obj& object) {
	ui::MenuRecipe().apply(object);
	object.spawn_child< make_close_button>();
}
struct CloseGameOnCloseButton:ecs::System {
	ecs::EventReader<ui::NoMenus> reader;
	void run(ecs::Ecs& world) {
		if (reader.read().nonempty()) {
			world.write_command(ui::open_menu(player::player_for(world).get_component<CloseMenuComponent>().button));
		}
		ecs::View<ui::InteractionState, ecs::Has<CloseButton>> view(world);
		for (auto[interaction]:view) {
			if (interaction.left_clicked) {
				world.write_command(core::CloseGameCommand());
			}

		}

	}
};
inline void CloseMenuPlugin(core::App& app) {
	app.emplace_system<CloseGameOnCloseButton>();
}