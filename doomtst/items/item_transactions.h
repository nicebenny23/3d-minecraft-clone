#include "Item.h"
#include "transaction.h"
#pragma once
namespace items {

	struct give_stack_plan {
		give_stack_plan(ecs::obj stack_from, ecs::obj stack_to, size_t amount) :from(stack_from), to(stack_to), count(amount) {

		}
		ecs::obj from;
		ecs::obj to;
		size_t count;
		void apply(ecs::Ecs& world) {
			item_stack& to_stack = to.get_component<item_stack>();
			item_stack& from_stack = from.get_component<item_stack>();

			if (!from_stack.can_give_to(to_stack, count)) {
				stn::throw_domain_error("plan was not validated");
			}
			from_stack.give(to_stack, count);
		}
	};

	inline stn::Option<give_stack_plan> give_stack_all(item_stack& from_stack, item_stack& to_stack, size_t amount) {
		ecs::obj from = from_stack.owner();
		ecs::obj to = to_stack.owner();

		if (from_stack.can_give_to(to_stack, amount)) {
			return give_stack_plan(from, to, amount);
		}
		return stn::None;
	}

	inline stn::Option<give_stack_plan> transfer_stack_some(ecs::Constrained<item_stack> from_stack, ecs::Constrained<item_stack> to_stack) {
	
		size_t fit = from_stack.get<item_stack>().max_fit(to_stack.get<item_stack>());
		if (fit != 0) {
			return give_stack_plan(from_stack.object(), to_stack.object(), fit);
		}
		return stn::None;
	}

		inline stn::Option<give_stack_plan> transfer_stack_all(ecs::Constrained<item_stack> from_stack, ecs::Constrained<item_stack> to_stack) {
		if (from_stack.get<item_stack>().can_transfer(to_stack.get<item_stack>())) {
			return give_stack_plan(from_stack.object(), to_stack.object(), from_stack.get<item_stack>().count());
		}
		return stn::None;
	}
}
