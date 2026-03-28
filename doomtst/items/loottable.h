#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include "item_type.h"
#include "Item.h"
#include "slot_transactions.h"
#include "container_transactions.h"
#include "../player/playerinventory.h"
#include "../util/pair.h"
#pragma once 
namespace items {
	struct loot_element {


		items::item_entry entry;
		loot_element(item_id id, size_t count, items::item_types& types) :entry(id, count, types) {

		}
		loot_element(std::string id, size_t count, items::item_types& types) :entry(types.from_name(id), count, types) {

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
		virtual LootDrops drops_for(items::item_types& types) = 0;
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
		loot_tables(ecs::Ecs& world):types(world.ensure_resource<item_types>()) {

		}
		item_types& types;
	};
	struct  loot_dropper : ecs::component {
		stn::Option<ecs::entity> drop_to;
		loot_dropper(loot_table_id id) :table(id) {
		}
		void start() {


		}
		~loot_dropper() {


		};
		loot_table_id table;
		loot_tables& tables() {
			return world().get_resource<loot_tables>();
		}
		void destroy_hook() {
			if (drop_to) {
				for (const loot_element& element : tables().get(table).elements) {
					player::inventory& inv = world().get_component< player::inventory>(drop_to.unwrap());
					stn::Option<items::AddToSlotPlan> slot = items::give_container_entry(element.entry, inv.hotbar.get<items::container>());
					if (slot) {
						slot.unwrap().apply(world());
					}
				}
			}
		}
	};
	template<LootTableType T>
	void loot_table_recipe(ecs::obj& entity) {
		entity.add_component<loot_dropper>(entity.world().insert_resource<loot_tables>().insert<T>());
	}

}

namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<items::loot_dropper> = {
		.updates = false
	};
}

	// !loottable_HPP
