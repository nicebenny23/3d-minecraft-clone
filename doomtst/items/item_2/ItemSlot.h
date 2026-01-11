#include "ItemUi.h"
#include "Item.h"
#include "transaction.h"
#pragma once
namespace items {

	struct container_index {
		v2::Coord2 index;
		container_index(size_t x, size_t y) :index(x, y) {

		}

	};
	struct SlotIndex :ecs::component {
		container_index index;
		SlotIndex(container_index ind) :index(ind) {
		}
	};
	struct ElementSlot :ecs::component {
		bool occupied() const {
			return ecs::has_children(owner());
		}
		bool empty() const {
			return !occupied();
		}

		Option<ecs::entity> element() {
			if (occupied()) {
				return ecs::children(owner())[0];
			}
			return None;
		}


	};
	struct ItemSlotSpawner :ecs::Recipe {
		ItemSlotSpawner(container_index spawn_index) :index(spawn_index) {

		}
		container_index index;
		void apply(ecs::obj& entity) {
			entity.add_component<SlotIndex>(index);
		}
	};
	struct ItemDecal :ecs::component {
		ItemDecal(ecs::obj decal):item_decal_object(decal) {

		}
		ecs::obj item_decal_object;
	};
	struct ItemSlotUiSpawner {
		ItemSlotSpawner item_slot;
		void apply(ecs::obj& entity) {
		ecs::obj item_decal=ecs::spawn_child< ui::ui_image_spawner>(entity, renderer::TexturePath("idk_yet", "3"), geo::Box2d::origin_centered(v2::Vec2(1, 1)), 20);
		entity.ensure_component<ItemDecal>(item_decal);
			item_slot.apply(entity);
		}
	};
	struct swap_items:Transaction{
		ecs::obj slot1;
		ecs::obj slot2;
		bool validate(const ecs::Ecs& world) const{
			return slot1.has_component< ElementSlot>() && slot2.has_component< ElementSlot>();
		}
		void apply(ecs::Ecs& world) {
			slot1.get_component<ecs::Parent>().swap_children(slot2.get_component<ecs::Parent>());
		}
	};


}