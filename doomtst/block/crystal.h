#pragma once

#include "block_registry.h"
#include "../items/loottable.h"
namespace items {

	struct crystal_item : item_type {
		std::string name() const{
			return "crystal";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(renderer::TexturePath("images\\crystal.png"));
		}
	};
	struct crystal_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element(types.insert<crystal_item>(),1,types) });
		}
	};
}
namespace blocks {
	struct CrystalBlock :BlockType {
		void apply(ecs::obj& block) const {
			
			block.apply_recipe(items::loot_table_recipe<items::crystal_loot_table>);
		}
		std::string name() const override {
			return std::string("crystal");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits(4,2);
		}
		BlockTraits traits()const {
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, crystaloretex),true,4);
		}
	};
}