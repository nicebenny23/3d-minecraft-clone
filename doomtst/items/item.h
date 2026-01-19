#include "item_type.h"
#pragma once
namespace items {



	struct item_entry {
		
		item_entry(item_id Id, size_t cnt) :id(Id), count(cnt){

		}

		bool empty() const {
			return count == 0;
		}
		item_id id;
		size_t  count;
	};

	inline bool can_interact(const item_entry& itm1, const item_entry& itm2) {
		return itm1.id == itm2.id;
	}


	inline void assert_interactable(const item_entry& itm1, const item_entry& itm2) {
		if (!can_interact(itm1, itm2)) {
			stn::throw_logic_error("Invariant Broken : differing item types {} and {} cannot interact", itm1.id, itm2.id);
		};
	}

	struct item_stack :ecs::component {
	
		size_t get_count();
		size_t get_max();

		item_stack(item_entry entry,size_t max_items) :entry(entry), capacity_constraint(max_items) {
			if (capacity_constraint<=entry.count) {
				stn::throw_logic_error("trying to initilize an item stack with {} out of a max of {} items",entry.count,max_items);
			}
		};
		item_entry contained_entry() const {
			return entry;
		}
		item_id contained_id() const {
			return entry.id;
		}

		size_t count() const {
			return entry.count;
		}
		bool empty() const {
			return entry.empty();
		}
		bool full() const {
			return count() == capacity_constraint;
		}
		size_t rem_capacity() const {
			return capacity_constraint - entry.count;
		}
		bool can_fit(size_t count) const {
			return count <= rem_capacity();
		}
		bool can_fit(item_entry ent) const {
			return can_interact(ent,entry)&&can_fit(ent.count);
		}
		bool can_remove(size_t count) const {
			return count <= entry.count;
		}
		size_t max_fit(const item_entry& oth) const {
			if (!can_interact(entry, oth)) {
				return 0;
			}
			return rem_capacity();
		}
		size_t max_fit(const item_stack& oth) const {
			return max_fit(oth.entry);
		}
		void add(size_t count) {
			if (!can_fit(count)) {
				stn::throw_logic_error("Add error: attempting to add {} items to stack with {} items, exceeding its capacity of {} items", count, entry.count, capacity_constraint);
			}
			entry.count += count;
		}
		void remove(size_t count) {
			if (!can_remove(count)) {
				stn::throw_logic_error("Add error: attempting to add {} items to stack with {} items, exceeding its capacity of {} items", count, entry.count, capacity_constraint);
			}
			entry.count -= count;
		}

		bool can_give_to(const item_stack& other_item, size_t amt) const {
			return other_item.can_fit(amt) && can_remove(amt) && can_interact(entry, other_item.entry);

		}
		bool can_transfer(const item_stack& other_item) const {
			return can_give_to(other_item, count());

		}

		void transfer(item_stack& other_item) {
			give(other_item, entry.count);
		}
		void give(item_stack& other_item, size_t count) {

			if (!can_remove(count)) {
				stn::throw_logic_error("Transfer error: trying to transfer {} items from a stack with only {} items.", count, entry.count);
			}

			assert_interactable(entry,other_item.entry);
			if (other_item.rem_capacity() < count) {
				throw std::logic_error("Unable to transfer more items overflow its capacity");
			}
			other_item.entry.count += count;
			entry.count-= count;
		}

	private:	
		item_entry entry;
		size_t capacity_constraint;
	};


	struct item_durability : ecs::component {
		explicit item_durability(size_t cap) : remaining(cap), max_durability(cap) {
		}

		size_t max_dur() const {
			return max_durability;
		}
		size_t durability() const {
			return remaining;
		}

		bool is_broken() const {
			return remaining == 0;
		}
		bool can_use() const {
			return remaining != 0;
		}
		bool is_full() const {
			return remaining == max_durability;
		}

		size_t damage_taken() const {
			return max_durability - remaining;
		}

		void use() {
			if (is_broken()) {
				throw std::logic_error("Item already broken, cannot damage further.");
			}
			--remaining;
		}
	private:
		size_t max_durability;
		size_t remaining;
	};
	inline bool can_remove(const item_stack& from, const item_stack& to, size_t amount) {
		return from.owner().get_component<item_stack>().can_give_to(to.owner().get_component<item_stack>(), amount);
	}

	inline bool can_transfer(const item_stack& from, const item_stack& to) {
		return can_remove(from, to, from.owner().get_component<item_stack>().count());
	}
	struct ItemSpawner:ecs::Recipe{
		item_entry entry;
		ItemSpawner(item_entry initial_state) :entry(initial_state) {
		}
		void apply(ecs::obj& entity) {
			item_traits traits = entity.world().ensure_resource<item_type_register>().from_id(entry.id);
			if (traits.use_type == item_use_type::block) {
				entity.add_component<item_stack>(entry,64);
			}
			else {
				entity.add_component<item_stack>(entry,1);
			}
		}
	};
	//clears all empty items
	struct ItemClear :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<item_stack> stack_query(world);
			for (const auto& [slot] : stack_query) {
				if (slot.count() == 0) {
					slot.owner().destroy();
				}
			}
			ecs::View<item_durability> dur_query(world);
			for (const auto& [slot] : dur_query) {
				if (slot.is_broken() == 0) {
					slot.owner().destroy();
				}
			}

		}

	};

}