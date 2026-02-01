#include "item_type.h"

#pragma once
namespace items {



	struct item_entry {
		
		item_entry(item_id Id, size_t cnt,const item_types& types) :id(Id), count(cnt){
			if (types.capacity_for(id)<cnt) {
				stn::throw_logic_error("size overweights {} capacity of {}",count,types.capacity_for(id));
			}

		}

		item_id id;
		size_t  count;
		
		size_t effective_capacity(const item_types& types) const {
			return types.capacity_for(id);
		}
	
		bool empty() const {
			return count==0;
		}
		bool full(const item_types& types) const {
			return count == types.capacity_for(id);
		}
		size_t rem_capacity(const item_types& types) const {
			return effective_capacity(types) - count;
		}
		bool can_fit(size_t fit_count, const item_types& types) const {
			return fit_count <= rem_capacity(types);
		}
		bool can_fit(item_entry ent, const item_types& types) const {
			return can_interact(ent, *this) && can_fit(ent.count, types);
		}
		bool can_remove(size_t remove_count, const item_types& types) const {
			return  remove_count <= count;
		}
		size_t max_fit(const item_entry& oth, const item_types& types) const {
			if (!can_interact(*this, oth)) {
				return 0;
			}
			return rem_capacity(types);
		}
		void add(size_t add_count,const item_types& types) {
			if (!can_fit(add_count,types)) {
				stn::throw_logic_error("Add error: attempting to add {} items to stack with {} items, exceeding its capacity of {} items", count, add_count, effective_capacity(types));
			}
			count += add_count;
		}
		void remove(size_t remove_count, const item_types& types) {
			if (!can_remove(remove_count,types)) {
				stn::throw_logic_error("Add error: attempting to add {} items to stack with {} items, exceeding its capacity of {} items", count, count, effective_capacity(types));
			}
			count -= remove_count;
		}

		bool can_give_to(const item_entry& other_item, size_t amt, const item_types& types) const {
			return other_item.can_fit(amt, types) && can_remove(amt, types) && can_interact(*this, other_item);

		}
		bool can_transfer(const item_entry& other_item, const item_types& types) const {
			return can_give_to(other_item, count,types);

		}

		void transfer(item_entry& other_item, const item_types& types) {
			give(other_item, count,types);
		}
		void give(item_entry& other_item, size_t give_count, const item_types& types) {

			if (!can_remove(count, types)) {
				stn::throw_logic_error("Transfer error: trying to transfer {} items from a stack with only {} items.", give_count, count);
			}

			assert_interactable(*this, other_item);
			if (other_item.rem_capacity(types) < give_count) {
				throw std::logic_error("Unable to transfer more items overflow its capacity");
			}
			other_item.count += give_count;
			count -= give_count;
		}
		static void assert_interactable(const item_entry& itm1, const item_entry& itm2) {
			if (!can_interact(itm1, itm2)) {
				stn::throw_logic_error("Invariant Broken : differing item types {} and {} cannot interact", itm1.id, itm2.id);
			};
		}

		static bool can_interact(const item_entry& itm1, const item_entry& itm2) {
			return itm1.id == itm2.id;
		}

	};


	
	struct item_stack :ecs::component {
	

		item_stack(item_entry new_entry) :entry(new_entry) {
		};
		item_entry contained_entry() const {
			return entry;
		}
		item_id contained_id() const {
			return entry.id;
		}
		const item_types& types() const{
			return world().get_resource<item_types>().expect("item types must exist");
		}
		size_t count() const {
			return entry.count;
		}
		bool empty() const {
			return entry.empty();
		}
		bool full() const {
			return entry.full(types());
		}
		size_t rem_capacity() const {
			return entry.rem_capacity(types());
		}
		bool can_fit(size_t count) const {
			return entry.can_fit(count,types());
		}
		bool can_fit(item_entry ent) const {
			return entry.can_fit(ent, types());
		}
		bool can_remove(size_t count) const {
			return count <= entry.count;
		}
		size_t max_fit(const item_entry& oth) const {
			return entry.max_fit(oth, types());
		}
		size_t max_fit(const item_stack& oth) const {
			return max_fit(oth.entry);
		}
		void set(item_entry new_entry) {
			entry = new_entry;
		}
		void add(size_t count) {
			entry.add(count, types());
		}
		void remove(size_t count) {
			entry.remove(count, types());
		}

		bool can_give_to(const item_stack& other_item, size_t amt) const {
			return entry.can_give_to(other_item.entry, amt, types());

		}
		bool can_transfer(const item_stack& other_item) const {
			return entry.can_transfer(other_item.entry, types());

		}

		void transfer(item_stack& other_item) {
			entry.transfer(other_item.entry, types());
		}
		void give(item_stack& other_item, size_t count) {
			entry.give(other_item.entry, count, types());
		}

	private:	
		item_entry entry;
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
			item_traits traits = entity.world().ensure_resource<item_types>().from_id(entry.id);
				entity.ensure_component<item_stack>(entry).set(entry);
			
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