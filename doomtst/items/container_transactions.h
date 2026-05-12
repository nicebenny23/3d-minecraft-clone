#include "Container.h"
#include "slot_transactions.h"
#pragma once
namespace items {
	//no need yet to add the interface
	struct AddContainerPlans {
		stn::array<AddToSlotPlan> plans;
		size_t count() const {
			size_t counter = 0;
			for (const AddToSlotPlan& slot_plan : plans) {
				counter += slot_plan.entry.count;
			}
			return counter;
		}
		AddContainerPlans(stn::array<AddToSlotPlan> slot_plans) :plans(slot_plans) {

		}
		void apply() {
			for (AddToSlotPlan& slot_plan : plans) {
				slot_plan.apply();
			}
		}
	};
	template<typename T> requires ecs::ConstrainedBy<std::ranges::range_value_t<T>,ElementSlot>
	inline stn::Option<AddContainerPlans> give_container_entry(item_entry ent, T& recieving) {
		size_t remaining_amount = ent.count;
		stn::Option < AddToSlotPlan > unfilled_slot;
		stn::array<AddToSlotPlan > filled_slots;
		for (auto& to_handle : recieving) {
			
				stn::Option<AddToSlotPlan> slot_plan = AddToSlotPlan::build(AddToSlotRequest{ .entry = ent, .slot = to_handle.object() });
				if (slot_plan) {
					if (slot_plan.unwrap().will_initialize()) {
						if (!unfilled_slot) {
							unfilled_slot = slot_plan;
						}
					}
					else {
						filled_slots.push(slot_plan.unwrap());
						remaining_amount -= filled_slots.last().entry.count;
						if (remaining_amount == 0) {
							return AddContainerPlans(std::move(filled_slots));
						}
					}
				}
		}

		if (unfilled_slot) {
			unfilled_slot.unwrap().entry.count = remaining_amount;
			filled_slots.push(unfilled_slot.unwrap());
		}
		if (filled_slots.non_empty()) {
			return AddContainerPlans(std::move(filled_slots));
		}
		return stn::None;
	}

}
