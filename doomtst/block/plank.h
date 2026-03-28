#include "../items/loottable.h"
#include "block_registry.h"
#pragma once 
namespace blocks {

	struct crafting_table_loot_table :items::LootTable {
		items::LootDrops drops_for(items::item_types& types) {
			return items::LootDrops({ items::loot_element("plank",1,types) });
		};
	};
	struct PlankBlock :BlockType {
		void apply(ecs::obj& block)  const override 
		{
			block.apply_recipe(items::loot_table_recipe<items::crafting_table_loot_table>);
		}
		std::string name() const{
			return std::string("plank");
		}
		BlockTraits traits() const {
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, planktex, planktex, planktex, planktex, planktex, planktex));
		}
	};
}
namespace items {
	struct plank_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				std::string_view("plank"),
				renderer::TexturePath("images\\treestoneblock.png", "PlankTexture")
			,world.get_resource<BlockRegistry>().get_id<blocks::PlankBlock>());
		}
	};
}