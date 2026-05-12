#include "Item.h"
#include "transaction.h"
#pragma once
namespace items {
	
	
	struct ItemStackAction {

		stn::Option<ecs::Constrained<item_stack>&> from;
		stn::Option< ecs::Constrained<item_stack>&> to;
		item_entry entry; 
		void apply(ecs::Ecs& world) {
			if (from) {
				item_stack& from_stack = from.unwrap().get<item_stack>();
				from_stack.remove(entry.count);
			}
			if (to) {
				item_stack& to_stack = from.unwrap().get<item_stack>();
				to_stack.add(entry.count);
			}
		}
	};
	struct ItemStackRequest {
		stn::Option<ecs::Constrained<item_stack>&> from;
		stn::Option< ecs::Constrained<item_stack>&> to;
		item_entry entry;
		bool need_complete;
		stn::Option<ItemStackAction> build() const {
			size_t result=entry.count;
			if (to) {
				stn::set_min(result,to.unwrap().get<item_stack>().max_fit(entry));
			};
			if (from) {
				item_stack& from_stack= from.unwrap().get<item_stack>();
				if (item_entry::can_interact(from_stack.contained_entry(), entry)) {
					stn::set_min(result,from_stack.count());
				}
			}
			if (result!=entry.count&&need_complete||result==0) {
				return stn::None;
			}
			item_entry real_entry = entry;

			real_entry.count = result;
			return ItemStackAction{.from = from,.to = to,.entry = real_entry };
		}
	};

}
