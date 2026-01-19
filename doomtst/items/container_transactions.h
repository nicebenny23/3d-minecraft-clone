#include "Container.h"
#include "slot_transactions.h"
#pragma once
namespace items {
	struct take_plans :Plan {
		stn::array<GiveToSlot> plans;
		size_t count() const {
			size_t counter = 0;
			for (const GiveToSlot& slot_plan : plans) {
				counter += slot_plan.count;
			}
			return counter;
		}
		take_plans(stn::array<GiveToSlot> slot_plans) :plans(slot_plans) {

		}
		void apply(ecs::Ecs& world) {
			for (GiveToSlot& slot_plan : plans) {
				slot_plan.apply(world);
			}
		}
	};


	//transfers as many as possible
	struct give_container {

		size_t amount;
		ecs::obj item_slot_from;
		ecs::obj container_to;
		stn::Option<take_plans> plan(const ecs::Ecs& world) {

			ElementSlot& from_slot=item_slot_from.get_component<ElementSlot>();
			if (from_slot.empty()) {
				return stn::None;
			}
			size_t remaining_amount = amount;
			container& to = container_to.get_component<container>();
			stn::Option<GiveToSlot> unfilled_slot;
			stn::array<GiveToSlot> filled_slots;
			for (ecs::object_handle& to_handle : to) {
				if (to_handle.has_component<ElementSlot>()) {
					stn::Option<GiveToSlot> slot_plan = transfer_slot_some(to_handle.get_component<ElementSlot>(), from_slot, remaining_amount).plan(world);
					if (slot_plan) {
						if (slot_plan.unwrap().will_initialize()) {
							if (!unfilled_slot) {
								unfilled_slot = slot_plan;
							}
						}
						else {
							filled_slots.push(slot_plan.unwrap());
							remaining_amount -= filled_slots.last().count;
							if (remaining_amount == 0) {
								return take_plans(std::move(filled_slots));
							}
						}
					}
				}
			}
			if (unfilled_slot) {
				unfilled_slot.unwrap().count = remaining_amount;
				filled_slots.push(unfilled_slot.unwrap());
			}
			if (filled_slots.nonempty()) {
				return take_plans(std::move(filled_slots));
			}
			return stn::None;
		}
		give_container(ElementSlot& slot, container& recieving_container,size_t amount_to_give):item_slot_from(slot.owner()),container_to(recieving_container.owner()), amount(amount_to_give){
		}
	};
	struct transfer_container {
		ecs::obj item_slot_from;
		ecs::obj container_to;
		transfer_container(ElementSlot& slot, container& recieving_container) :item_slot_from(slot.owner()), container_to(recieving_container.owner()) {
		}
		stn::Option<take_plans> plan(const ecs::Ecs& world) {
			ElementSlot& from_slot = item_slot_from.get_component<ElementSlot>();
			if (from_slot.empty()) {
				return stn::None;
			}
			return give_container(from_slot, container_to.get_component<container>(), from_slot.element().unwrap().get_component<item_stack>().count()).plan(world);
		}
	};

	struct give_container_entry {

		item_entry entry;
		ecs::obj container_to;
		stn::Option<AddToSlot> plan(ecs::Ecs& world) {

			container& to = container_to.get_component<container>();
			for (ecs::object_handle& handle: to) {
				stn::Option< AddToSlot> slot= AddSlot(handle.get_component<ElementSlot>(),entry).plan(world);
				if (slot) {
					return slot;
				}
			}
			return stn::None;
		}
		give_container_entry(item_entry ent, container& recieving_container):entry(ent),container_to(recieving_container.owner()) {
			int l = 2;
		}
	};
}
