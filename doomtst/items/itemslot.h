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
	struct ElementSlot :ecs::component {
		stn::Option<ecs::weak_object> current_item;
		bool occupied() const{
			return current_item.is_some_and(&ecs::weak_object::alive);
		}
		bool empty() const{
			return !occupied();
		}
		ElementSlot() {

		}
		bool can_interact(const ElementSlot& other) const{
			if (other.occupied()&&occupied()) {
				return item_entry::can_interact(other.entry().unwrap(),entry().unwrap());
			}
			return false;
		}
		void set_element(ecs::obj elem) {
			current_item = ecs::weak_object(elem);
		}
		void reset_element() {
			current_item = stn::None;
		}
		void clear() {
			if (occupied()) {
				element().unwrap().destroy();
			}
			current_item = stn::None;
		}
		Option<ecs::obj> element(){
			if (occupied()) {
				return current_item.unwrap().get();
			}
			return stn::None;
		}
		Option<const ecs::obj> element() const{
			if (occupied()) {
				return current_item.unwrap().get();
			}
			return stn::None;
		}
		Option<const item_stack&> stack() const{
			if (empty()) {
				return stn::None;
			}
			return element().unwrap().get_component<item_stack>();
		}
		Option<item_stack&> stack() {
			if (empty()) {
				return stn::None;
			}
			return element().unwrap().get_component<item_stack>();
		}
		Option<item_entry> entry() const{
			return stack().map_member(&item_stack::contained_entry);
		}
	};
	struct ItemSlotSpawner :ecs::Recipe {
		ItemSlotSpawner(){

		}
		void apply(ecs::obj& entity) {
			entity.add_component<ElementSlot>();
		}
	};
}