
#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace items {

	struct StoneItem : item_type {
		std::string name() const {

			return ("stone");
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(
				renderer::TexturePath("images\\stone.png"),
				world.get_resource<BlockRegistry>().get_id("stone")
			);
		}
	};
	struct stone_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element(types.insert<StoneItem>(),1,types) });
		}
	};
	struct StoneBricksItem : item_type {
		std::string name() const {

			return ("stone_brick");
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(
				renderer::TexturePath("images\\stone_brick.png"),
				world.get_resource<BlockRegistry>().get_id("stone_brick")
			);
		}
	};
	struct stone_brick_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element(types.insert<StoneBricksItem>(),1,types) });
		}
	};
}
namespace blocks {
	struct StoneBrickBlock :BlockType {
		void apply(ecs::obj& block) const {
			block.apply_recipe(items::loot_table_recipe<items::stone_brick_loot_table>);
		}
		//we can catche here for a preformance gain
	
		std::string name() const override {
			return std::string("stone_brick");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits(4, 0);
		}
		BlockMeshTraits traits(BlockTextureRegistry& registry)const {
			return  BlockMeshTraits(v3::unit_scale, false, registry.get_texture("images\\stone_brick.png"));
		}
	};
	struct StoneBlock :BlockType {
		void apply(ecs::obj& block) const {
			if (!catched_loot_id) {
				catched_loot_id = block.world().get_resource<items::loot_tables>().insert<items::stone_loot_table>();
			}
			block.add_component_unchecked<items::loot_dropper>(catched_loot_id.unwrap());
		}
		//we can catche here for a preformance gain
		mutable stn::Option<items::loot_table_id> catched_loot_id;
		StoneBlock(ecs::Ecs& world){

		}
		std::string name() const override {
			return std::string("stone");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits(4,0);
		}
		BlockMeshTraits traits(BlockTextureRegistry &registry)const {
			return  BlockMeshTraits(v3::unit_scale, false, registry.get_texture("images\\stone.png"));
		}
	};
}