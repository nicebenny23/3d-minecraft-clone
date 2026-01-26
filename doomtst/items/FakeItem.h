#include "ItemSlot.h"
#include "SlotUi.h"
#pragma once
namespace items {
	struct display_item :ecs::component {
		stn::Option<item_entry> display;

	};
	struct DisplaySlot :ecs::Recipe {
		DisplaySlot(v2::Coord2 inv_loc) : inventory_location(inv_loc) {

		}
		v2::Coord2 inventory_location;
		void apply(ecs::obj& entity) {
			GriddedItemSlotUISpawner(inventory_location).apply(entity);	
			entity.add_component<display_item>();
			ItemUiSpawner().apply(entity);
		}
	};

	struct SyncDisplayIcon :ecs::System {
		
			void run(ecs::Ecs& world) {

				for (auto&& [icon, item] : ecs::View<ItemIcon, display_item>(world)) {
					icon.displayed_id = item.display.member(&item_entry::id).copied();
				}

				for (auto&& [count, item] : ecs::View<ItemCountDisplay, display_item>(world)) {
					count.count= item.display.member(&item_entry::count).copied();
				}
			}
	};
}