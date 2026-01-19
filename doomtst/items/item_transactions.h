#include "Item.h"
#include "transaction.h"
#pragma once
namespace items{

	struct give_stack_plan :Plan {
		give_stack_plan(ecs::obj stack_from, ecs::obj stack_to,size_t amount) :from(stack_from), to(stack_to),count(amount) {

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
	struct give_stack_all {
		ecs::obj from;
		ecs::obj to;
		size_t count;
		give_stack_all(item_stack& from_stack, item_stack& to_stack,size_t amount):from(from_stack.owner()),to(to_stack.owner()),count(amount){
		}
		stn::Option<give_stack_plan> plan(const ecs::Ecs& world) {

			const item_stack& to_stack = to.get_component<item_stack>();
			const item_stack& from_stack = from.get_component<item_stack>();
			if (from_stack.can_give_to(to_stack,count)) {
				return give_stack_plan(from, to, count);
			}
			return stn::None;
		}
	};
	struct transfer_stack_some {
		ecs::obj from;
		ecs::obj to;
		transfer_stack_some(item_stack& from_stack, item_stack& to_stack, size_t amount) :from(from_stack.owner()), to(to_stack.owner()) {
		}
		stn::Option<give_stack_plan> plan(const ecs::Ecs& world) {

			const item_stack& to_stack = to.get_component<item_stack>();
			const item_stack& from_stack = from.get_component<item_stack>();
			if (from_stack.max_fit(to_stack)!=0) {
				return give_stack_plan(from, to, from_stack.max_fit(to_stack));
			}
			return stn::None;
		}
	};
	
	struct transfer_stack_all {
		ecs::obj from;
		ecs::obj to;
		transfer_stack_all(item_stack& from_stack, item_stack& to_stack, size_t amount) :from(from_stack.owner()), to(to_stack.owner()) {
		}
		stn::Option<give_stack_plan> plan(const ecs::Ecs& world) {

			const item_stack& to_stack = to.get_component<item_stack>();
			const item_stack& from_stack = from.get_component<item_stack>();
			if (from_stack.can_transfer(to_stack)) {
				return give_stack_plan(from, to,from_stack.count());
			}
			return stn::None;
		}
	};
}