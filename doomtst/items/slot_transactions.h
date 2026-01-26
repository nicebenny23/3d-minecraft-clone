#include "ItemSlot.h"
#include "item_transactions.h"
#pragma once
namespace items {

	struct SetItemSlot {
		ecs::obj slot;
		item_entry entry;
		SetItemSlot(ecs::obj item_slot,item_entry new_entry) :entry(new_entry), slot(item_slot) {
		}
		void apply(ecs::Ecs& world) {
			ElementSlot& element_slot=slot.get_component<ElementSlot>();
			if (element_slot.empty()) {
				element_slot.current_item = slot.spawn_child<DisplayedItemSpawner>(entry);
			}
			else {
				element_slot.element().unwrap().get_component<item_stack>().set(entry);
			}
		}
	};

	struct AddToSlotPlan {
		ecs::obj slot;
		item_entry entry;
		AddToSlotPlan(ecs::obj item_slot, item_entry new_entry) :entry(new_entry), slot(item_slot) {
		}
		void apply(ecs::Ecs& world) {

			if (slot.get_component<ElementSlot>().empty()) {
				SetItemSlot(slot, entry).apply(world);
			}
			else {
				item_stack& stack = slot.get_component<ElementSlot>().element().expect("item should exist").get_component<item_stack>();
				if (!item_entry::can_interact(stack.contained_entry(), entry)) {
					stn::throw_logic_error("AddToSlot was not validated");
				}
				stack.add(entry.count);
			}
		}
	};

	struct TakeFromSlot {
		ecs::obj slot;
		item_entry entry;
		TakeFromSlot(item_entry new_entry, ecs::obj item_slot) :entry(new_entry), slot(item_slot) {
		}
		void apply(ecs::Ecs& world) {

			item_stack& stack = slot.get_component<ElementSlot>().element().expect("slot must not be empty").get_component<item_stack>();
			if (!item_entry::can_interact(stack.contained_entry(), entry)) {
				stn::throw_logic_error("AddToSlot was not validated");
			}
			stack.remove(entry.count);
		}
	};

	struct GiveToSlot {
		//element slot
		ecs::obj from_slot;
		//element slot
		ecs::obj to_slot;
		size_t count;
		bool will_initialize()  {
			return to_slot.get_component<ElementSlot>().empty();
		}
		GiveToSlot(ElementSlot& from, ElementSlot& to, size_t cnt) :from_slot(from.owner()), to_slot(to.owner()), count(cnt) {
		}
		void apply(ecs::Ecs& world) {

			ecs::obj item_entity = from_slot.get_component<ElementSlot>().element().expect("plan should have been validated");

			ecs::obj to_entity = to_slot.get_component<ElementSlot>().element().unwrap_or_else([&] {
				item_id id = item_entity.get_component<item_stack>().contained_id();
				SetItemSlot( to_slot, item_entry(id, count)).apply(world);
				return to_slot.get_component<ElementSlot>().element().unwrap();
				});
			give_stack_plan(item_entity, to_entity, count).apply(world);
		}
	};


	inline stn::Option<AddToSlotPlan> AddToSlot(ElementSlot& to, item_entry amount) {
			stn::Option<ecs::obj> element = to.element();
			if (element) {
				if (!element.unwrap().get_component<item_stack>().can_fit(amount)) {
					return stn::None;
				}
			}
			return AddToSlotPlan(to.owner(), amount);
		}
	

		inline stn::Option<GiveToSlot> give_slot_all(ElementSlot& from_slot, ElementSlot& to_slot, size_t amount) {
			if (from_slot.empty()) {
				return stn::None;
			}
			const item_stack& from_stack = from_slot.element().unwrap().get_component<item_stack>();
			size_t from_stack_count = from_stack.count();
			if (from_stack_count < amount) {
				return stn::None;
			}
			stn::Option<ecs::obj> mabye_slot = to_slot.element();
			if (!mabye_slot) {
				return GiveToSlot(from_slot, to_slot, amount);
			}
			const item_stack& other = mabye_slot.unwrap().get_component<item_stack>();
			if (other.can_fit(item_entry(from_stack.contained_id(),amount))) {
				return GiveToSlot(from_slot, to_slot,amount );
			}
			return stn::None;
		}
	
	inline stn::Option<GiveToSlot> transfer_slot_all(ElementSlot& from_slot, ElementSlot& to_slot){
		return from_slot.element().and_then([&](ecs::obj item_entity) {
			return give_slot_all(from_slot, to_slot, item_entity.get_component<item_stack>().count());
			});
	}
	struct swap_slot_plan{
		//element slot
		ecs::obj from;
		//element slot
		ecs::obj to;
		swap_slot_plan(ecs::obj from_obj, ecs::obj to_obj) :from(from_obj), to(to_obj) {

		}
		void apply(ecs::Ecs& world) {
			stn::Option<ecs::obj> to_child = to.get_component <ElementSlot>().element();
			stn::Option<ecs::obj> from_child = from.get_component <ElementSlot>().element();

			to.get_component<ElementSlot>().reset_element();
			from.get_component<ElementSlot>().reset_element();

			if (to_child) {
				from.add_child(to_child.unwrap());
				from.get_component <ElementSlot>().set_element(to_child.unwrap());
			}
			if (from_child) {
				to.add_child(from_child.unwrap());
				to.get_component <ElementSlot>().set_element(from_child.unwrap());
			}
		}
	};
	inline swap_slot_plan swap_slot(ElementSlot& from_slot, ElementSlot& to_slot) {
		return swap_slot_plan(from_slot.owner(), to_slot.owner());
	}
}