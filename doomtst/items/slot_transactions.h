#include "ItemSlot.h"
#include "item_transactions.h"
#pragma once
namespace items {

	struct MakeItemSlot :Plan {
		ecs::obj slot;
		item_entry entry;
		MakeItemSlot(item_entry new_entry, ecs::obj item_slot) :entry(new_entry), slot(item_slot) {
			int l = 1;
		}
		void apply(ecs::Ecs& world) {
			ElementSlot& element_slot=slot.get_component<ElementSlot>();
			if (element_slot.occupied()) {
				stn::throw_logic_error("slot not validated");
			}
			element_slot.current_item=slot.spawn_child<ItemUiSpawner>(entry);
		}
	};
	struct AddToSlot :Plan {
		ecs::obj slot;
		item_entry entry;
		AddToSlot(item_entry new_entry, ecs::obj item_slot) :entry(new_entry), slot(item_slot) {
			int l = 3;
		}
		void apply(ecs::Ecs& world) {

			if (slot.get_component<ElementSlot>().empty()) {
				MakeItemSlot(entry, slot).apply(world);

			}
			item_stack& stack = slot.get_component<ElementSlot>().element().expect("item should exist").get_component<item_stack>();
			if (!can_interact(stack.contained_entry(), entry)) {
				stn::throw_logic_error("AddToSlot was not validated");
			}
			stack.add(entry.count);
		}
	};

	struct TakeFromSlot :Plan {
		ecs::obj slot;
		item_entry entry;
		TakeFromSlot(item_entry new_entry, ecs::obj item_slot) :entry(new_entry), slot(item_slot) {
		}
		void apply(ecs::Ecs& world) {

			item_stack& stack = slot.get_component<ElementSlot>().element().expect("slot must not be empty").get_component<item_stack>();
			if (!can_interact(stack.contained_entry(), entry)) {
				stn::throw_logic_error("AddToSlot was not validated");
			}
			stack.remove(entry.count);
		}
	};

	struct GiveToSlot :Plan {
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
				MakeItemSlot(item_entry(id, count), to_slot).apply(world);
				return to_slot.get_component<ElementSlot>().element().unwrap();
				});
			give_stack_plan(item_entity, to_entity, count).apply(world);
		}
	};


	struct AddSlot {
		//element slot

		ecs::obj to_slot;
		item_entry amount;
	
		AddSlot(ElementSlot& to, item_entry cnt) :to_slot(to.owner()), amount(cnt) {
		}
		stn::Option<AddToSlot> plan(ecs::Ecs& world) {
			stn::Option<ecs::obj> element = to_slot.get_component<ElementSlot>().element();
			if (element) {
				if (!element.unwrap().get_component<item_stack>().can_fit(amount)) {
					return stn::None;
				}
			}
			return AddToSlot(amount,to_slot);
		}
	};

	struct give_slot_some {
		//element slot
		ecs::obj from;
		//element slot
		ecs::obj to;
		size_t count;
		give_slot_some(ElementSlot& from_slot, ElementSlot& to_slot, size_t amount) :from(from_slot.owner()), to(to_slot.owner()), count(amount) {

		}
		stn::Option<GiveToSlot> plan(const ecs::Ecs& world) {
			ElementSlot& from_slot = from.get_component<ElementSlot>();

			ElementSlot& to_slot = from.get_component<ElementSlot>();
			if (from_slot.empty()) {
				return stn::None;
			}
			const item_stack& stack = from_slot.element().unwrap().get_component<item_stack>();
			size_t stack_count = stack.count();
			if (stack_count < count) {
				return stn::None;
			}
			stn::Option<ecs::obj> mabye_slot = to_slot.element();
			if (!mabye_slot) {
				return GiveToSlot(from_slot, to_slot, count);
			}
			const item_stack& other = mabye_slot.unwrap().get_component<item_stack>();
			if (can_interact(other.contained_entry(), stack.contained_entry())) {
				return GiveToSlot(from_slot, to_slot, other.rem_capacity());
			}
			return stn::None;
		}
	};

	struct transfer_slot_some {
		//element slot
		ecs::obj from;
		//element slot
		ecs::obj to;
		transfer_slot_some(ElementSlot& from_slot, ElementSlot& to_slot, size_t amount) :from(from_slot.owner()), to(to_slot.owner()) {

		}
		stn::Option<GiveToSlot> plan(const ecs::Ecs& world) {
			ElementSlot& from_slot = from.get_component<ElementSlot>();
			return from_slot.element().and_then([&](ecs::obj item_entity) {
				return give_slot_some(from_slot, to.get_component<ElementSlot>(), item_entity.get_component<item_stack>().count()).plan(world);
				});
		}
	};

	struct swap_slot_plan {
		//element slot
		ecs::obj from;
		//element slot
		ecs::obj to;

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
	struct swap_slot {
		//element slot
		ecs::obj from;
		//element slot
		ecs::obj to;
		swap_slot(ElementSlot& from_slot, ElementSlot& to_slot) :from(from_slot.owner()), to(to_slot.owner()) {

		}
		stn::Option<swap_slot_plan> plan(const ecs::Ecs& world) {
			return swap_slot_plan(to, from);
		}
	};
}