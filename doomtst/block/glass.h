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
		BlockMeshTraits traits(BlockTextureRegistry& registry) const {
			
			return BlockMeshTraits(v3::Scale3::from_scale(1), true, registry.get_texture("images\\glass.png"));
		}
	};

}
namespace items {
	struct GlassItem : item_type {
		std::string name() const {
			return "glass";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(
				renderer::TexturePath("images\\glass.png"),
				world.get_resource<blocks::BlockRegistry>().get_id<blocks::GlassBlock>()
			);
		}
	};


}