#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace blocks {

	struct glass_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element("glass",1,types) });
		}
	};
	struct GlassBlock :BlockType {
		GlassBlock() = default;
		void apply(ecs::obj& block) const override {
			block.apply_recipe(items::loot_table_recipe<glass_loot_table>);
		}
		std::string name() const {
			return std::string("glass");
		}
		BlockTraits traits() const {
			return BlockTraits(
				BlockMeshTraits(v3::Scale3(1), true, glasstex, glasstex, glasstex, glasstex, glasstex, glasstex)
				, true);
		}
	};

}
namespace items {
	struct glass_item : item_type {
		std::string name() const {
			return "glass";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\glass.png"),
				world.get_resource<blocks::BlockRegistry>().get_id<blocks::GlassBlock>()
			);
		}
	};


}