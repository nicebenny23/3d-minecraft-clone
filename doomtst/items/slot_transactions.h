#include "ItemSlot.h"
#include "item_transactions.h"
#include "FakeItem.h"
#include "Item.h"
#pragma once
namespace items {
	struct AddToSlotPlan; 
	struct AddToSlotRequest {
		bool want_complete=false;
		item_entry entry;
		ecs::Constrained<ElementSlot> slot;
		stn::Option<AddToSlotPlan> build();
	};

	inline void swap_slot_plans(ecs::Constrained<ElementSlot> from, ecs::Constrained<ElementSlot> to) {
		stn::Option<ecs::Constrained<item_stack>> to_child = to.get <ElementSlot>().element();
		stn::Option<ecs::Constrained<item_stack>> from_child = from.get<ElementSlot>().element();

		to.get<ElementSlot>().reset_element();
		from.get<ElementSlot>().reset_element();

		if (to_child) {
			from.get<ElementSlot>().set_element(to_child.unwrap().object());
		}
		if (from_child) {
			to.get<ElementSlot>().set_element(from_child.unwrap().object());
		}
	}
	struct AddToSlotPlan {
		ecs::Constrained<ElementSlot> slot;
		item_entry entry;
		
		bool will_initialize() const {
			return slot.get<ElementSlot>().empty();
		}
		void apply() {
			if (slot.get_component<ElementSlot>().empty()) {
				slot.get_component<ElementSlot>().set_element(ecs::spawn(slot.world(), ItemSpawner(entry)));
			}
			else {
				item_stack& stack = slot.get<ElementSlot>().stack().unwrap();
				if (!item_entry::can_interact(stack.contained_entry(), entry)) {
					stn::throw_logic_error("AddToSlot was not validated");
				}
				stack.add(entry.count);
			}
		}
		static stn::Option<AddToSlotPlan> build(AddToSlotRequest request) {
			stn::Option<item_stack&> element = request.slot.get<ElementSlot>().stack();
			size_t new_count = request.entry.count;
			if (element) {
				stn::set_min(new_count,element.unwrap().max_fit(request.entry));
			}
			if (request.entry.count!= new_count&&request.want_complete) {
				return stn::None;
			}
			request.entry.count = new_count;
			if (new_count==0) {
				return stn::None;
			}
			return AddToSlotPlan(request.slot, request.entry);
		}
	private:
		AddToSlotPlan(ecs::Constrained<ElementSlot> item_slot, item_entry new_entry) :entry(new_entry), slot(item_slot) {
		}
	};
	inline stn::Option<AddToSlotPlan> items::AddToSlotRequest::build() {
		return AddToSlotPlan::build(*this);
	}
	struct RemoveFromSlotPlan;
	struct RemoveFromSlotRequest {
		bool want_complete = false;
		ecs::Constrained<ElementSlot> slot;
		size_t count;
		stn::Option<RemoveFromSlotPlan> build();
	};
	struct RemoveFromSlotPlan {
		ecs::Constrained<ElementSlot> slot;
		size_t count;

		void apply() {
			item_stack& stack = slot.get_component<ElementSlot>().element().expect("slot must not be empty").get<item_stack>();
			stack.remove(count);
		}
		static stn::Option<RemoveFromSlotPlan> build(RemoveFromSlotRequest request) {
			stn::Option<item_stack&> stack= request.slot.get<ElementSlot>().stack();
			if (!stack) {
				return stn::None;
			}
			size_t max_remove =stn::min(stack.unwrap().count(),request.count);
			if (request.want_complete&&max_remove!=request.count) {
				return stn::None;
			}
			return RemoveFromSlotPlan(request.slot, request.count);
		}
	private:
		RemoveFromSlotPlan(ecs::Constrained<ElementSlot> elem_slot, size_t remove_count) :slot(elem_slot), count(remove_count) {
		}
	};

	inline stn::Option<RemoveFromSlotPlan> RemoveFromSlotRequest::build() {
		return RemoveFromSlotPlan::build(*this);
	}
	struct TransferSlotsPlan;
	static constexpr stn::NoneType max_transfer = stn::None;
	struct TransferSlotsRequest {
		ecs::Constrained<ElementSlot> from_slot;
		ecs::Constrained<ElementSlot> to_slot;
		stn::Option<size_t> count= max_transfer;
		bool want_complete = false;
		stn::Option<TransferSlotsPlan> build();
	};

	struct TransferSlotsPlan {
		AddToSlotPlan add;
		RemoveFromSlotPlan take;
		bool will_initialize() {
			return add.will_initialize();
		}
		
		void apply() {
			add.apply();
			take.apply();
		}
		static stn::Option<TransferSlotsPlan> build(TransferSlotsRequest request) {
			stn::Option<item_stack&> from_object = request.from_slot.get<ElementSlot>().stack();
			if (!from_object) {
				return stn::None;
			}

			size_t wanted_transfer_amount = request.count.unwrap_or(from_object.unwrap().count());
			size_t transfer_amount = wanted_transfer_amount;
			stn::set_min(transfer_amount, from_object.unwrap().count());
			item_id id = from_object.unwrap().contained_id();
			stn::Option<items::item_stack&> mabye_to_slot = request.to_slot.get<ElementSlot>().stack();
			if (mabye_to_slot) {
				if (mabye_to_slot.unwrap().contained_id()!=id) {
					return stn::None;
				}
				stn::set_min(transfer_amount, mabye_to_slot.unwrap().rem_capacity());
			}
			if (request.want_complete&& wanted_transfer_amount!=transfer_amount) {
				return stn::None;
			}
			item_entry req_amt(id,transfer_amount,request.from_slot.world().get_resource<ItemTypes>());
			stn::Option<AddToSlotPlan> add=AddToSlotPlan::build(AddToSlotRequest{ .entry = req_amt,.slot = request.to_slot });
			stn::Option<RemoveFromSlotPlan> remove=RemoveFromSlotPlan::build(RemoveFromSlotRequest{ .slot = request.from_slot,.count = transfer_amount });
			if (add&&remove) {
				return TransferSlotsPlan(add.unwrap(), remove.unwrap());
			}
			return stn::None;
			
		}
		private: 
			TransferSlotsPlan(AddToSlotPlan add, RemoveFromSlotPlan take):add(add),take(take){
		}
	};
	inline stn::Option<TransferSlotsPlan> items::TransferSlotsRequest::build() {
		return TransferSlotsPlan::build(*this);
	}
	template<typename T>
	inline bool apply_plan(T request) {
		auto plan = request.build();
		if (plan) {
			
			plan.unwrap().apply();
		}
		return plan.is_some();
	}
	
}