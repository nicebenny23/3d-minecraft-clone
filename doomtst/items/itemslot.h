#include "ItemUi.h"
#include "Item.h"
#include "transaction.h"
#include "../game/ecs/weak_object.h"
#pragma once
namespace items {

	struct ElementSlot :ecs::component {
		stn::Option<ecs::WeakObject> current_item;
		bool occupied() const{
			return current_item.is_some_and(&ecs::WeakObject::alive);
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
			current_item = ecs::WeakObject(elem);
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
		void destroy_hook() {
			if (element()) {
				element().unwrap().destroy();
			}
		}
		Option<ecs::Constrained<item_stack>> element() const{
			if (occupied()) {
				return ecs::Constrained<item_stack>(current_item.unwrap().get().unwrap());
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