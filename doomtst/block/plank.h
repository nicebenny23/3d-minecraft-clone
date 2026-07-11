#include "../items/loottable.h"
#include "block_registry.h"
#pragma once 
namespace blocks {

	struct plank_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element("plank",1,types) });
		};
	};
	struct PlankBlock :BlockType {
		void apply(ecs::obj& block)  const override 
		{
			block.apply_recipe(items::loot_table_recipe<plank_loot_table>);
		}
		std::string name() const{
			return std::string("plank");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits(3, 0);
		}
		BlockMeshTraits traits(BlockTextureRegistry& registry) const {
			block_texture plank_texture= registry.get_texture("images\\plank.png");
		return BlockMeshTraits(v3::unit_scale, false, plank_texture);
		}
	};
	struct log_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element("plank",4,types)});
		};
	};
	struct LogBlock :BlockType {
		void apply(ecs::obj& block)  const override {
			block.apply_recipe(items::loot_table_recipe<log_loot_table>);
		}
		std::string name() const {
			return std::string("log");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits(3,0);
		}
		BlockMeshTraits traits(BlockTextureRegistry& textures) const {
			block_texture log_side = textures.get_texture("images\\treestone.png");
			block_texture logtoppng = textures.get_texture("images\\log.png");

			return BlockMeshTraits(v3::unit_scale, false, log_side, log_side,logtoppng, logtoppng, log_side, log_side);
		}
	};
}
namespace items {
	struct PlankItem : item_type {
		std::string name() const {

			return "plank";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(
				renderer::TexturePath("images\\plank.png")
			,world.get_resource<BlockRegistry>().get_id<blocks::PlankBlock>());
		}
	};
}