#include "Container.h"
#include "slot_transactions.h"
#pragma once
namespace items {
	struct take_plans  {
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

	inline stn::Option<take_plans> give_container(ElementSlot& from_slot, container& recieving_container, size_t amount_to_give) {

		if (from_slot.empty()) {
			return stn::None;
		}
		size_t remaining_amount = amount_to_give;
		stn::Option<GiveToSlot> unfilled_slot;
		stn::array<GiveToSlot> filled_slots;
		for (ecs::object_handle& to_handle : recieving_container) {
			if (to_handle.has_component<ElementSlot>()) {
				stn::Option<GiveToSlot> slot_plan = give_slot_some(to_handle.get_component<ElementSlot>(), from_slot, remaining_amount);
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
	inline stn::Option<take_plans> transfer_container(ElementSlot& from_slot, container& recieving_container) {
		if (from_slot.empty()) {
			return stn::None;
		}
		return give_container(from_slot, recieving_container, from_slot.element().unwrap().get_component<item_stack>().count());
	}

	inline stn::Option<AddToSlotPlan> give_container_entry(item_entry ent, container& recieving_container) {

		for (ecs::object_handle& handle : recieving_container) {
			stn::Option< AddToSlotPlan> slot = AddToSlot(handle.get_component<ElementSlot>(), ent);
			if (slot) {
				return slot;
			}
		}
		return stn::None;
	}

}
