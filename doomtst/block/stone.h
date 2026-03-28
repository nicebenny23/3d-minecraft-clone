
#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace items {

	struct stone_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				std::string_view("stone"),
				renderer::TexturePath("images\\stone.png", "StoneTexture"),
				world.get_resource<BlockRegistry>().get_id("stone")
			);
		}
	};
	struct stone_loot_table :items::LootTable {
		items::LootDrops drops_for(items::item_types& types) {
			return items::LootDrops({ items::loot_element(types.from_type<stone_item>(),1,types) });
		}
	};
}
namespace blocks {
	struct StoneBlock :BlockType {
		void apply(ecs::obj& block) const {
			block.apply_recipe(items::loot_table_recipe<items::stone_loot_table>);
		}
		std::string name() const override {
			return std::string("stone");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits{ .time_to_mine = 3 };
		}
		BlockTraits traits()const {
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, stonetex));
		}
	};
}