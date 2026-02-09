#include "ItemSlot.h"
#include "SlotUi.h"
#include "../game/ecs/weak_object.h"
#pragma once
namespace items {
	struct RefrencedSlot :ecs::component {
		stn::Option<item_entry> entry() {
			return slot
				.get_component<ElementSlot>()
				.entry();
		}
		ElementSlot& displayed() {
			return slot.get_component<ElementSlot>();
		}
		ecs::obj slot;
		RefrencedSlot(ecs::obj to_refrence) :slot(to_refrence) {

		}
	};

	struct DisplayedItemSpawner :ecs::Recipe {

		DisplayedItemSpawner() {
		}
		void apply(ecs::obj& entity) {

			entity.add_component<DisplayItem>();
			ItemUiSpawner().apply(entity);
		}
	};
	struct RefrencedSlotSpawner :ecs::Recipe {
		ecs::obj refrence_to;
		RefrencedSlotSpawner(ecs::obj& refrenced_slot) :refrence_to(refrenced_slot) {
		}
		void apply(ecs::obj& entity) {

			DisplayedItemSpawner().apply(entity);
			entity.add_component<RefrencedSlot>(refrence_to);
		}
	};
	struct ClearItemSlotSpawner:ecs::Recipe {
		ecs::obj refrence_to;
		ClearItemSlotSpawner(ecs::obj& refrenced_slot) :refrence_to(refrenced_slot) {
		}
		void apply(ecs::obj& entity) {
			ui::UiSpawner(inventory_transform_floating(v2::zerov), 221).apply(entity);
			RefrencedSlotSpawner(refrence_to).apply(entity);
		}
	};
	struct ItemSlotDispaySpawner :ecs::Recipe {
		v2::Coord2 location;
		ecs::obj slot;
		ItemSlotDispaySpawner(ecs::obj slt, v2::Coord2 loc) :location(loc), slot(slt) {
		}
		void apply(ecs::obj& object) {
			ui::UiSpawner(inventory_transform_floating(inventory_center(location)), 0).apply(object);
			ecs::obj item_decal = object.spawn_child<ui::ui_image_spawner>(renderer::TexturePath("images\\blockholder.png", "item_decal_original"), geo::Box2d(v2::zerov, v2::unitv), 20);
			object.add_component<ItemSlotDecal>(item_decal);
			RefrencedSlotSpawner(slot).apply(object);
		}
	};
	struct SyncDisplayIcon :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [item, refrence] : ecs::View<DisplayItem, RefrencedSlot>(world)) {
				item.display = refrence.entry();
			}
		}
	};
}