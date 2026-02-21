#include "../items/container_transactions.h"
#include "../game/ecs/game_object.h"
#include "../util/userinput.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../items/container_ui.h"
#include "../items/crafting.h"
#include "../game/ecs/filtered_object.h"
#include "player.h"
#pragma once 
namespace player {




	struct inventory : ecs::component {
		void givestartitems(stn::array<std::string>& items) {
			for (const std::string& item : items) {
				items::item_id id = world().get_resource<items::item_types>().from_name(item);
				items::item_entry entry = items::item_entry(id, 10, world().insert_resource<items::item_types>());
				stn::Option<items::AddToSlotPlan> plan = items::give_container_entry(entry, hotbar.get_component < items::container>());
				if (plan) {
					plan.unwrap().apply(world());
				}
			}
		}
		inventory(ecs::Constrained<items::container> slots_, ecs::Constrained<items::container> hotbar_, ecs::Constrained<items::ContainerDisplay> hotbar_display_)
			:slots(slots_), hotbar_display(hotbar_display_), hotbar(hotbar_) {


		};
		ecs::Constrained<items::container> slots;
		ecs::Constrained<items::ContainerDisplay> hotbar_display;
		ecs::Constrained<items::container> hotbar;
		stn::Option<items::container_index> selected_ind;
		stn::Option<items::item_stack&> selected() {
			return selected_object()
				.map_member(&ecs::obj::get_component<items::item_stack>);
		}
		stn::Option<ecs::obj> selected_object() {
			if (selected_ind == stn::None) {
				return stn::None;
			}
			return hotbar.get_component<items::container>()[selected_ind.unwrap()]
				.get_component<items::ElementSlot>()
				.element();
		}
		void update() {

		}

	};

}

// !playerinventory_H
