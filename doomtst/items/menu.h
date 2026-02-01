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
		open_menu(menu_component& menu_comp):menu_ent(menu_comp.owner()){

		}
		ecs::obj menu_ent;
	};
	struct MenuState :ecs::resource {
		bool menu_open() const{
			return open_menu.is_some();
		}
		MenuState() = default;
		stn::Option<ecs::obj> open_menu;
	};

	struct MenuEnabler:ecs::System {
		void run(ecs::Ecs& world) {
			MenuState& state=world.ensure_resource<MenuState>();

			if (world.get_resource<userinput::InputManager>().unwrap().getKey(userinput::escape_key).pressed) {
				world.write_command<close_menu>(close_menu());
			}

			for (open_menu menu : world.read_commands<open_menu>()) {
				
				menu.menu_ent.get_component<ui::UiEnabled>().enable();
				state.open_menu = menu.menu_ent;
				return;
			}
			for (close_menu menu:world.read_commands<close_menu>() ) {
				if (state.menu_open()) {
					state.open_menu.unwrap().get_component<ui::UiEnabled>().disable();
					state.open_menu = stn::None;
					return;
				}
				else {
					//close app or somthing;
				}
			}


		}
		
	
	};
	struct MenuPlugin :Core::Plugin{
		void build(Core::App& app) {
			app.ensure_resource< MenuState>();
			app.emplace_system<MenuEnabler>();
		  }
	};






}