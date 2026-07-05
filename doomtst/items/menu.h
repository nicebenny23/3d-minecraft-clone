#pragma once
#include "recipe_transactions.h"
namespace ui {
	struct CloseMenu {
		CloseMenu() {

		};
	};
	struct MenuComponent :ecs::component {

	};
	
	struct NoMenus {

	};
	struct menu_stack {
		menu_stack(ecs::Constrained<MenuComponent> menu_comp) :menu_ent(menu_comp) {

		}
		ecs::Constrained<MenuComponent> menu_ent;
	};
	struct MenuRecipe {
		MenuRecipe() {

		}
		void apply(ecs::obj& object) const{
			object.add_component<MenuComponent>();
			UiSpawner(geo::Box2d(v2::zerov, v2::unitv), 1).apply(object);
			object.get_component<ui::UiEnabled>().disable();
		}
	};
	struct MenuState :ecs::resource {
		bool menu_open() const {
			return menu_stack.non_empty();
		}
		bool no_menu_open() const {
			return !menu_open();
		}
		stn::Option<ecs::obj> top() const{
			if (menu_open()) {
				return menu_stack.peek();
			}
			return stn::None;

		}
		MenuState() = default;
		mutable stn::stack<ecs::obj> menu_stack;
	};
	inline bool is_open(ecs::Constrained<MenuComponent> menu) {
		return menu.world().get_resource< MenuState>().top()==menu.object();
	}
	
	struct MenuEnabler :ecs::System {
		void run(ecs::Ecs& world) {
			MenuState& state = world.ensure_resource<MenuState>();

			if (world.get_resource<userinput::InputManager>().key(userinput::escape_key).pressed) {
				world.write_command<CloseMenu>(CloseMenu());
			}
			//on expire
		

			for (menu_stack menu : world.read_commands<menu_stack>()) {
				if (state.top() != menu.menu_ent.object()) {
	
					if (state.menu_open()) {
						state.top().unwrap().get_component<ui::UiEnabled>().disable();
					}
					menu.menu_ent.get_component<ui::UiEnabled>().enable();
					state.menu_stack.push(menu.menu_ent.object());
				}
			}
			for (CloseMenu menu : world.read_commands<CloseMenu>()) {
				if (state.menu_open()) {
					state.top().unwrap().get_component<ui::UiEnabled>().disable();
					state.menu_stack.pop();
					if (state.menu_open()) {
						state.top().unwrap().get_component<ui::UiEnabled>().enable();
					}
					return;
				}
				else {

					world.emplace_event<NoMenus>();
				}
			}
		}
	};

	struct MenuPlugin {
		void operator()(core::App& app) {
			app.ensure_resource< MenuState>();
			app.emplace_system<MenuEnabler>();
		}
	};

}