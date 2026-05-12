#pragma once
#include "recipe_transactions.h"
namespace ui {
	struct close_menu {
		close_menu() {

		};
	};
	struct menu_component :ecs::component {

	};
	

	struct open_menu {
		open_menu(ecs::Constrained<menu_component> menu_comp) :menu_ent(menu_comp) {

		}
		ecs::Constrained<menu_component> menu_ent;
	};
	struct MenuRecipe {
		MenuRecipe() {

		}
		void apply(ecs::obj& object) const{
			object.add_component<menu_component>();
			UiSpawner(geo::Box2d(v2::zerov, v2::unitv), 1).apply(object);
			object.get_component<ui::UiEnabled>().disable();
		}
	};
	struct MenuState :ecs::resource {
		bool menu_open() const {
			return open_menu.non_empty();
		}
		bool no_menu_open() const {
			return !menu_open();
		}
		stn::Option<ecs::obj> top() const{
			if (menu_open()) {
				return open_menu.peek();
			}
			return stn::None;

		}
		MenuState() = default;
		mutable stn::stack<ecs::obj> open_menu;
	};
	inline bool is_open(ecs::Constrained<menu_component> menu) {
		return menu.world().get_resource< MenuState>().top()==menu.object();
	}
	struct MenuEnabler :ecs::System {
		void run(ecs::Ecs& world) {
			MenuState& state = world.ensure_resource<MenuState>();

			if (world.get_resource<userinput::InputManager>().key(userinput::escape_key).pressed) {
				world.write_command<close_menu>(close_menu());
			}
			//on expire
		

			for (open_menu menu : world.read_commands<open_menu>()) {
				if (state.top() != menu.menu_ent.object()) {
	
					if (state.menu_open()) {
						state.top().unwrap().get_component<ui::UiEnabled>().disable();
					}
					menu.menu_ent.get_component<ui::UiEnabled>().enable();
					state.open_menu.push(menu.menu_ent.object());
				}
			}
			for (close_menu menu : world.read_commands<close_menu>()) {
				if (state.menu_open()) {
					state.top().unwrap().get_component<ui::UiEnabled>().disable();
					state.open_menu.pop();
					if (state.menu_open()) {
						state.top().unwrap().get_component<ui::UiEnabled>().enable();
					}
					return;
				}
				else {
					world.emplace_command<Core::CloseGameCommand>();
				}
			}
		}
	};
	struct MenuPlugin {
		void operator()(Core::App& app) {
			app.ensure_resource< MenuState>();
			app.emplace_system<MenuEnabler>();
		}
	};

}