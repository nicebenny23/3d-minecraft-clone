#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include"item_transactions.h"
#include "../util/random.h"

#pragma once 
const float interacttimeneededfordrop = 1;
struct loot_element {


	items::item_entry entry;

	void drop(ecs::Ecs& world) const;
	loot_element(items::item_entry item_entry):entry(item_entry){

	}

};
struct  loot_table : ecs::component {

	bool should_drop;
	loot_table():should_drop(false),loot(){
	}
	void start() {


	}
	~loot_table() {


	};
	stn::array<loot_element> loot;
	void add(stn::HashedString item_name, size_t maxamt, bool israndom = false) {
		items::item_types& types = world().ensure_resource<items::item_types>();
		loot.push(loot_element(items::item_entry(types.from_name(item_name), maxamt, types)));
	}
	void add(std::string_view item_name, size_t maxamt, bool israndom = false) {
		add(stn::HashedString(item_name),maxamt,israndom);
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
