#include "recipe_transactions.h"
#include "FakeItem.h"
#include "cursor_slot.h"
#include "menu.h"
#include "Container.h"
#include "ItemSlot.h"
#include "ItemUi.h"
#include "recipe.h"
#include "../renderer/ui.h"
#include "../util/Option.h"
#include "../game/ecs/filtered_object.h"
#pragma once
namespace items {
	


	struct crafter :ecs::component {
		crafter(RecipeBinder binder) :binder(binder), wanted(stn::None) {

		}

		void start() {
		}
		RecipeBinder binder;
		stn::Option<ItemRecipe> wanted;
		void set_state() {
			wanted=best_booklet_recipe(binder.list, binder.input.get_component<container>(), stn::None);

		}
	};
	struct crafting_slot_displayer :ecs::component {
		ecs::Constrained<crafter> crafter_comp;
		ecs::Constrained<DisplayItem,ui::InteractionState> display;
		crafting_slot_displayer(ecs::Constrained <crafter > craft, ecs::Constrained <DisplayItem, ui::InteractionState> display_item) :crafter_comp(craft), display(display_item){

		}
	};
	struct CraftingSlotDisplaySpawner {
		v2::Coord2 pos;
		ecs::Constrained<crafter> crafter;
		void apply(ecs::obj& entity) {
			ecs::Constrained<DisplayItem, ui::InteractionState> display = ecs::spawn_emplaced< FakeItemSlotDispaySpawner>(entity.world(),pos);
			entity.add_component <crafting_slot_displayer>(crafter, display);
		}

	};

	struct cursor_crafter :ecs::System {
		void run(ecs::Ecs& world) {
			if (world.get_resource<ui::MenuState>().no_menu_open()) {
				return;
			}
			ecs::obj cursor_obj = world.get_resource<items::cursor_container>().primary_slot();
			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			//		cursor_obj.get_component<ui::UiBounds>().local.center = world.ensure_resource<userinput::InputManager>().mouse_position;
			for (auto&& [crafting_slot_display] : ecs::View<ecs::With<items::crafting_slot_displayer>>(world)) {
				crafting_slot_display.display.get_component<DisplayItem>().display = crafting_slot_display.crafter_comp.get_component<crafter>().wanted.member(&ItemRecipe::output);
			}

			for (auto&& [ crafting_slot_display] : ecs::View<ecs::With<items::crafting_slot_displayer>>(world)) {
				if (crafting_slot_display.display.get_component<ui::InteractionState>().left_clicked) {
					crafter& crafter_comp = crafting_slot_display.crafter_comp.get_component<crafter>();
					auto auto_val = build_recipe_from_booklet(crafter_comp.binder.list, crafter_comp.binder.input.get_component<container>(), cursor_slot);
					if (auto_val) {
						auto_val.unwrap().apply(world);
					}
				}
			}
		}
	};

	struct run_crafts :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<ecs::With<crafter>> craft_view(world);
			for (auto&& [crafter_component] : craft_view) {
				crafter_component.set_state();
			}
		}
	};
}