#include "ItemUi.h"
#include "Item.h"
#include "transaction.h"
#include "../game/ecs/weak_object.h"
#pragma once
namespace items {

	struct container_index {
		v2::Coord2 coord;
		bool operator==(const container_index& other) const = default;
		container_index(size_t x, size_t y) :coord(x, y) {

		}

		size_t x() const{
			return coord.x;
		}
		size_t y() const {
			return coord.y;
		}
		bool fits_in(v2::Coord2 element_size) const{
			return x() <= element_size.x && y() <= element_size.y;
		}
	};
	struct SlotIndex :ecs::component {
		container_index index;
		SlotIndex(container_index ind) :index(ind) {
		}
	};
	struct ElementSlot :ecs::component {
		stn::Option<ecs::weak_object> current_item;
		bool occupied() const{
			return current_item.is_some_and(&ecs::weak_object::alive);
		}
		bool empty() {
			return !occupied();
		}
		ElementSlot() {

		}
		void set_element(ecs::obj elem) {
			current_item = ecs::weak_object(elem);
		}
		void reset_element() {
			current_item = stn::None;
		}

		Option<ecs::obj> element() {
			if (occupied()) {
				return current_item.unwrap().get();
			}
			return stn::None;
		}

		Option<item_stack&> stack() {
			if (empty()) {
				return stn::None;
			}
			return element().unwrap().get_component<item_stack>();
		}
	};
	struct ItemSlotSpawner :ecs::Recipe {
		ItemSlotSpawner(container_index spawn_index) :index(spawn_index) {

		}
		container_index index;
		void apply(ecs::obj& entity) {
			entity.add_component<SlotIndex>(index);
			entity.add_component<ElementSlot>();
		}
	};
}