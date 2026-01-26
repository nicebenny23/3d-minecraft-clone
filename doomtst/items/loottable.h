#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include"item_transactions.h"
#include "../util/random.h"

#pragma once 
const float interacttimeneededfordrop = 1;
struct loot_element {

	items::item_entry to_entry(const ecs::Ecs& world) const{
		
		items::item_id id= world.get_resource<items::item_type_register>().unwrap().from_name(item_name);
		return items::item_entry(id, amount);
	}
	loot_element() = default;
	std::string item_name;
	size_t amount;
	void drop(ecs::Ecs& world) const;
	loot_element(std::string name, float maxamt)
		: item_name(name), amount(maxamt) {

	}

};
struct  loot_table : ecs::component {

	bool should_drop;
	loot_table() {

		should_drop = false;
		loot = stn::array<loot_element>();
	}
	void start() {


	}
	~loot_table() {


	};
	stn::array<loot_element> loot;
	void add(std::string item_name, float maxamt, bool israndom = false) {
		loot.push(loot_element(item_name, maxamt));
	}
	void destroy_hook() {
		if (should_drop) {
			for (const loot_element& element:loot) {
				element.drop(world());
			}
		}
	}
};
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<loot_table> = {
		.updates = false
	};
}



// !loottable_HPP
