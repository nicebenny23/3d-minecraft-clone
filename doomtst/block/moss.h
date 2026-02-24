#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace blocks {
	struct moss_loot_table :items::LootTable {
		items::LootDrops drops_for(items::item_types& types) {
			return items::LootDrops({ items::loot_element("moss",1,types) });
		}
	};
	struct MossBlock :BlockType  {
		void apply(ecs::obj& block) const override {
			block.apply_recipe(items::loot_table_recipe<moss_loot_table>);
		}
		std::string name() const {
			return std::string("moss");
		}
		BlockTraits traits() const {
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, mosstex, mosstex, mosstex, mosstex, mosstex, mosstex)
			);
		}
	};

}