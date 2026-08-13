#include "ItemUi.h"
#include "Item.h"
#include "../game/ecs/weak_object.h"
#pragma once
namespace items {

	struct ElementSlot :ecs::component {
		ecs::WeakConstrained<item_stack> current_item;
		bool occupied() const{
			return current_item.alive();
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
			current_item = ecs::WeakConstrained<item_stack>(elem);
		}
		void reset_element() {
			current_item.reset();
		}
		void clear() {
			if (occupied()) {
				element().unwrap().destroy();
			}
			reset_element();
		}
		void destroy_hook() {
			clear();
		}
		Option<ecs::Constrained<item_stack>> element() const{
			if (occupied()) {
				return current_item.constrained().copied();
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
	struct ItemSlotSpawner {
		ItemSlotSpawner(){

		}
		void apply(ecs::obj& entity) const{
			entity.add_component<ElementSlot>();
		}
	};
}