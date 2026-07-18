#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include "item_type.h"
#include "Item.h"
#include "slot_transactions.h"
#include "container_transactions.h"
#include "../player/playerinventory.h"
#include "../util/pair.h"
#include "../game/time.h"
#pragma once 
namespace items {
	struct loot_element {

		double drop_probability=1;
		items::item_entry entry;
		loot_element(item_id id, size_t count, items::ItemTypes& types,double prob =1) :entry(id, count, types), drop_probability(prob) {

		}
		loot_element(std::string id, size_t count, items::ItemTypes& types) :entry(types.from_name(id), count, types) {

		}
		loot_element(items::item_entry item_entry) :entry(item_entry) {

		}

	};
	using LootDrops = stn::array<loot_element>;
	struct LootTable {
		LootTable() = default;
		LootDrops get(ecs::obj dropping) const {
			items::ItemTypes& types = dropping.world().get_resource<items::ItemTypes>();
			return drops_for(types, dropping);
		}
		virtual ~LootTable() = default;
		virtual LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const  = 0;
	};
	template<typename T>
	concept LootTableType = std::derived_from<T, LootTable>;
	using loot_table_id = stn::typed_id<LootDrops>;
	
	struct loot_tables:ecs::resource{
		stn::array<stn::box<LootTable>> drops;
		stn::type_indexer<loot_table_id> tables;
		template<LootTableType Type>
		loot_table_id insert() {
			stn::Insertion<loot_table_id> id = tables.insert<Type>();
			if (id.is_new) {
				drops.emplace(stn::construct_derived<Type>());
			}
			return id.value;
		}
		const LootTable& get(loot_table_id id) const{
			return *drops[id.id];
		}
		loot_tables(ecs::Ecs& world):types(world.ensure_resource<ItemTypes>()) {

		}
		ItemTypes& types;
	};
	struct  loot_dropper : ecs::component {
		stn::Option<ecs::entity> drop_to;
		//idk any better ones currently 
		ecs::obj self;
		loot_dropper(loot_table_id id) :table(id) {
		}
		void set(ecs::Constrained<player::inventory> object) {
			drop_to = object.object().inner();
			last_interaction_time = world().get_resource<timing::WorldClock>().elapsed_time;
		}
		double last_interaction_time;
		loot_table_id table;
		loot_tables& tables() {
			return world().get_resource<loot_tables>();
		}

		void start(ecs::entity entity) {
			self =ecs::obj( entity,world());
		}
		void destroy_hook() {
			if (drop_to) {
				double time_diff = world().get_resource<timing::WorldClock>().elapsed_time-last_interaction_time;
				double get_diff_time = 1.0;
				if (get_diff_time<time_diff) {
					return;
				}
				for (const loot_element& element : tables().get(table).get(self)) {
					if (random::random()<element.drop_probability) {
						player::inventory& inv = world().get_component< player::inventory>(drop_to.unwrap());
						stn::array<ecs::Constrained<items::ElementSlot>> slots;
						for (items::container_element& elem: inv.hotbar.get<items::Container>()) {
							slots.push(elem);
						}
						for (items::container_element& elem : inv.slots.get<items::Container>()) {
							slots.push(elem);
						}
						stn::Option<items::AddContainerPlans>  give_action= items::give_container_entry(element.entry, slots);
						if (give_action) {
							give_action.unwrap().apply();
						}
					}
				}
			}
		}
	};
	template<LootTableType T>
	void loot_table_recipe(ecs::obj& entity) {
		entity.add_component<loot_dropper>(entity.world().get_resource<loot_tables>().insert<T>());
	}


}

namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<items::loot_dropper> = {
		.updates = false
	};
}

	// !loottable_HPP
