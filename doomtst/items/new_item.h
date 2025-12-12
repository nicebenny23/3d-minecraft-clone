#include "../game/ecs/game_object.h"
#include <string>
#pragma once

namespace item_name {
	//represents the type of item an item is
	struct item :ecs::component {
		size_t id;
		enum class item_type {
			tool,
			block

		};
		item_type type;
		bool can_give(item& other, size_t cnt);
		size_t get_count();
		size_t get_max();
	};
	//bidirectional
	bool can_interact(const item& itm1, const item& itm2) {
		return itm1.id == itm2.id && itm1.type == itm2.type;
	}
	void make_item(ecs::obj& object, size_t id) {


	}
	item& get_item(ecs::obj object) {
		item* itm = object.get_component_ptr<item>();
		if (itm == nullptr) {
			throw std::logic_error("Error:tried to get item component but it did not exist");
		}
		return *itm;
	}
	item& get_item(ecs::component& comp) {
		item* itm = comp.owner().get_component_ptr<item>();
		if (itm == nullptr) {
			throw std::logic_error("Error:tried to get item component but it did not exist");
		}
		return *itm;
	}

	void ensure_interactable(item& itm1, item& itm2) {
		if (itm1.id != itm2.id) {
			std::string msg =
				"Invariant Broken: differing item types cannot interact ('" +
				std::to_string(itm1.id) + "' and '" + std::to_string(itm2.id) + "').";
		};
	}






	struct item_stack :ecs::component {
		item_stack(size_t max_items) :amt(0), cap(max_items) {
		};
		size_t count() const {
			return amt;
		}

		size_t capacity() const {
			return cap;
		}
		bool empty() const {
			return amt == 0;
		}
		bool full() const {
			return amt == cap;
		}
		size_t rem_capacity() const {
			return cap - amt;
		}
		bool can_fit(size_t count) const {
			return count <= rem_capacity();
		}
		bool can_give(size_t count) const {
			return count <= amt;
		}
		size_t max_fit(item_stack& oth) {
			if (!can_interact(get_item(owner()), get_item(oth))) {
				return 0;
			}
			return rem_capacity();
		}
		void add(size_t count) {
			if (!can_fit(count)) {
				std::string message =
					"Add error: attempting to add " + std::to_string(count) +
					" items to stack with " + std::to_string(amt) + " items, exceeding its capacity of " +
					std::to_string(cap) + ".";
				throw std::logic_error(message);
			}
			amt += count;
		}
		bool can_give(item_stack& other_item, size_t amt) {
			return other_item.can_fit(amt) && can_give(amt) && can_interact(get_item(owner()), get_item(other_item));

		}
		void pass(item_stack& other_item, size_t count) {

			if (!can_give(amt)) {
				std::string msg = "Transfer error: trying to transfer " + std::to_string(count) +
					" items from a stack with only " + std::to_string(amt) + " items.";
				throw std::logic_error(msg);
			}

			ensure_interactable(get_item(owner()), get_item(other_item));
			if (other_item.rem_capacity() < count) {
				throw std::logic_error("Unable to transfer more items overflow its capacity");
			}
			other_item.amt += count;
			amt -= count;
		}
		//returns an item_stack of size count;
		item& split(size_t count) {
			if (!can_give(amt)) {
				throw std::logic_error("Split error: trying to split " + std::to_string(count) +
					" items from a stack with only " + std::to_string(amt) + " items.");
			}
			ecs::obj new_item = owner().world()->spawn_empty();
			make_item(new_item, get_item(owner()).id);
			pass(new_item.get_component<item_stack>(), count);
			return get_item(new_item);
		}

	private:
		size_t cap;
		size_t amt;
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

		// How much durability has been lost (damage taken)
		size_t damage_taken() const {
			return max_durability - remaining;
		}

		// Use (damage) the item by 1 point
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

}