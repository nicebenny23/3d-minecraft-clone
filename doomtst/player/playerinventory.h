#include "../items/container_transactions.h"
#include "../game/ecs/game_object.h"
#include "../items/container_ui.h"
#include "../game/ecs/filtered_object.h"
#pragma once 
namespace player {




	struct inventory : ecs::component {
		void givestartitems(stn::array<std::string>& items) {
			for (const std::string& item : items) {
				items::item_id id = world().get_resource<items::ItemTypes>().from_name(item);
				items::item_entry entry = items::item_entry::try_max(id, 15, world().insert_resource<items::ItemTypes>());
				stn::Option<items::AddContainerPlans> plan = items::give_container_entry(entry, hotbar.get < items::container>());
				if (plan) {
					plan.unwrap().apply();
				}
			}
		}
		inventory(ecs::Constrained<items::container> slots_, ecs::Constrained<items::container> hotbar_, ecs::Constrained<items::ContainerDisplay> hotbar_display_)
			:slots(slots_), hotbar_display(hotbar_display_), hotbar(hotbar_) {


		};
		ecs::Constrained<items::container> slots;
		ecs::Constrained<items::ContainerDisplay> hotbar_display;
		ecs::Constrained<items::container> hotbar;
		stn::Option<v2::UVec2> selected_ind;
		stn::Option<items::item_stack&> selected() {
			return selected_object().map([](ecs::Constrained<items::item_stack> stack)->items::item_stack&{return stack.get<items::item_stack>(); });
		}
		stn::Option<ecs::Constrained<items::item_stack>> selected_object() {
			if (selected_ind == stn::None) {
				return stn::None;
			}
			return hotbar.get_component<items::container>()[selected_ind.unwrap()]
				.get_component<items::ElementSlot>()
				.element();
		}

	};

}

// !playerinventory_H
