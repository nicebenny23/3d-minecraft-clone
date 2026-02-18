#include "ItemSlot.h"
#include "SlotUi.h"
#include "../game/ecs/weak_object.h"
#include "../game/ecs/filtered_object.h"
#pragma once
namespace items {
	struct RefrencedSlot :ecs::component {
		stn::Option<item_entry> entry() {
			return displayed()
				.entry();
		}
		ElementSlot& displayed() {
			return slot.get_component<ElementSlot>();
		}
		ecs::Constrained< ElementSlot> slot;
		RefrencedSlot(ecs::Constrained< ElementSlot> to_refrence) :slot(to_refrence) {

		}
	};
	struct ClearItemSlotSpawner{
		ecs::Constrained< ElementSlot> refrence_to;
		ClearItemSlotSpawner(ecs::Constrained< ElementSlot> refrenced_slot) :refrence_to(refrenced_slot) {
		}
		void apply(ecs::obj& entity) {
			DisplayedItemSpawner().apply(entity);
			ui::UiSpawner(inventory_transform_floating(v2::zerov), 31000).apply(entity);
			entity.add_component<RefrencedSlot>(refrence_to);
		}
	};

	struct FakeItemSlotDispaySpawner {
		v2::Coord2 location;
		FakeItemSlotDispaySpawner(v2::Coord2 loc) :location(loc) {
		}
		void apply(ecs::obj& object) {
			DisplayedItemSpawner().apply(object);
			ui::UiSpawner(inventory_transform_floating(inventory_center(location)), 20).apply(object);
			ecs::obj item_decal = object.spawn_child<ui::ui_image_spawner>(renderer::TexturePath("images\\blockholder.png", "item_decal_original"), geo::Box2d(v2::zerov, v2::unitv), 0);
			object.add_component<ItemSlotDecal>(item_decal);
		}
	};
	struct ItemSlotDispaySpawner {
		v2::Coord2 location;
		ecs::Constrained< ElementSlot> slot;
		ItemSlotDispaySpawner(ecs::Constrained< ElementSlot> slt, v2::Coord2 loc) :location(loc), slot(slt) {
		}
		void apply(ecs::obj& object) {
			FakeItemSlotDispaySpawner(location).apply(object);
			object.add_component<RefrencedSlot>(slot);
		}
	};
	struct SyncDisplayIcon :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [item, refrence] : ecs::View<ecs::With<DisplayItem>, ecs::With<RefrencedSlot>>(world)) {
				item.display = refrence.entry();
			}
		}
	};
}