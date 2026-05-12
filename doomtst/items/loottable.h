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

		double drop_probibility=1;
		items::item_entry entry;
		loot_element(item_id id, size_t count, items::ItemTypes& types,double prob =1) :entry(id, count, types), drop_probibility(prob) {

		}
		loot_element(std::string id, size_t count, items::ItemTypes& types) :entry(types.from_name(id), count, types) {

		}
		loot_element(items::item_entry item_entry) :entry(item_entry) {

		}

	};

	struct LootDrops {
		LootDrops(const stn::array<loot_element>& drops) :elements(drops) {

		};
		LootDrops(const std::initializer_list<loot_element>& drops) :elements(drops) {

		};

		stn::array<loot_element> elements;
	};
	struct LootTable {
		LootTable() = default;
		virtual LootDrops drops_for(items::ItemTypes& types) = 0;
	};
	template<typename T>
	concept LootTableType = std::derived_from<T, LootTable>;
	using loot_table_id = stn::typed_id<LootDrops>;
	
	struct loot_tables:ecs::resource{
		stn::array<LootDrops> drops;
		stn::type_indexer<loot_table_id> tables;
		template<LootTableType Type>
		loot_table_id insert() {
			stn::insertion<loot_table_id> id = tables.insert<Type>();
			if (id.is_new) {
				drops.push(Type().drops_for(types));
			}
			return id.value;
		}
		LootDrops get(loot_table_id id) const{
			return drops[id.id];
		}
		loot_tables(ecs::Ecs& world):types(world.ensure_resource<ItemTypes>()) {

		}
		ItemTypes& types;
	};
	struct  loot_dropper : ecs::component {
		stn::Option<ecs::entity> drop_to;
		loot_dropper(loot_table_id id) :table(id) {
		}
		void start() {


		}
		~loot_dropper() {


		};
		void set(ecs::Constrained<player::inventory> object) {
			drop_to = object.object().inner();
			last_interaction_time = world().get_resource<timing::WorldClock>().elapsed_time;
		}
		double last_interaction_time;
		loot_table_id table;
		loot_tables& tables() {
			return world().get_resource<loot_tables>();
		}
		void destroy_hook() {
			if (drop_to) {
				double time_diff = world().get_resource<timing::WorldClock>().elapsed_time-last_interaction_time;
				if (1.0<time_diff) {
					return;
				}
				for (const loot_element& element : tables().get(table).elements) {
					if (random::random()<element.drop_probibility) {
						player::inventory& inv = world().get_component< player::inventory>(drop_to.unwrap());
						stn::array<ecs::Constrained<items::ElementSlot>> slots;
						for (items::container_element& elem: inv.hotbar.get<items::container>()) {
							slots.push(elem);
						}
						for (items::container_element& elem : inv.slots.get<items::container>()) {
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
