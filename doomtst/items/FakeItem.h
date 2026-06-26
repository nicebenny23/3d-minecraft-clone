#include "ItemSlot.h"
#include "SlotUi.h"
#include "../game/ecs/weak_object.h"
#include "../game/ecs/filtered_object.h"
#pragma once
namespace items {
	struct RefrencedSlot :ecs::component {
		stn::Option<item_entry> entry() {
			return displayed().get<ElementSlot>()
				.entry();
		}
		
		ecs::Constrained<ElementSlot> displayed() {
			return slot;
		}
		ElementSlot& displayed_slot() {
			return slot.get<ElementSlot>();
		}
		stn::Option<ecs::Constrained<item_stack>> displayed_object() {
			return displayed().get<ElementSlot>().element();
		}
		ecs::Constrained<ElementSlot> slot;
		RefrencedSlot(ecs::Constrained< ElementSlot> to_refrence) :slot(to_refrence) {

		}
	};
	struct ClearItemSlotSpawner{
		ecs::Constrained< ElementSlot> refrence_to;
		ClearItemSlotSpawner(ecs::Constrained< ElementSlot> refrenced_slot) :refrence_to(refrenced_slot) {
		}
		void apply(ecs::obj& entity) const{
			ui::UiSpawner(inventory_transform_floating(v2::zerov), 31000).apply(entity);
			entity.apply_recipe(ItemUiSpawner);
			entity.add_component<RefrencedSlot>(refrence_to);
		}
	};

	struct FakeItemSlotDispaySpawner {
		v2::Coord2 location;
		FakeItemSlotDispaySpawner(v2::Coord2 loc) :location(loc) {
		}
		void apply(ecs::obj& object) const{
			ui::UiSpawner(inventory_transform_floating(inventory_center(location)), 20).apply(object);
			object.apply_recipe(ItemUiSpawner);
			ecs::obj item_decal = object.spawn_child_emplaced<ui::ImageSpawner>(renderer::TexturePath("images\\blockholder.png"), geo::unit_box_2d, 0);
			object.add_component<ItemSlotDecal>(item_decal);
		}
	};
	struct ItemSlotDisplaySpawner {
		v2::Coord2 location;
		ecs::Constrained< ElementSlot> slot;
		ItemSlotDisplaySpawner(ecs::Constrained< ElementSlot> slt, v2::Coord2 loc) :location(loc), slot(slt) {
		}
		void apply(ecs::obj& object) const{
			FakeItemSlotDispaySpawner(location).apply(object);
			object.add_component<RefrencedSlot>(slot);
		}
	};
	struct SyncDisplayIcon :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [item_count, refrence] : ecs::View<ItemCountDisplay,RefrencedSlot>(world)) {
				item_count.count = refrence.entry().member(&item_entry::count);
			}
			for (auto&& [item_icon, refrence] : ecs::View<ItemIcon, RefrencedSlot>(world)) {
				item_icon.displayed_id= refrence.entry().member(&item_entry::id);
			}
			for (auto&& [item_progress, refrence] : ecs::View<ItemProgressDisplay, RefrencedSlot>(world)) {
				if (refrence.displayed_object().is_some_and(&ecs::Constrained<item_stack>::has_component<item_durability>)) {
					item_progress.value = refrence.displayed_object().unwrap().get_component<item_durability>().precent_left();
				}
				else{ 
					item_progress.value = stn::None;	
				}
			}
		}
		
	};
}