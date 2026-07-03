#pragma once

#include "block_registry.h"
#include "../items/loottable.h"
namespace items {
	
	struct DirtItem :item_type{

		std::string name() const {
			return "dirt";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(renderer::TexturePath("images\\dirt.png"),
				world.get_resource<BlockRegistry>().get_id("soil"));
		}

	};


	struct soil_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element(types.insert<DirtItem>(),1,types) });
		}
	};

}
namespace blocks {
	struct SoilBlock :BlockType {
		void apply(ecs::obj& block) const {

			block.apply_recipe(items::loot_table_recipe<items::soil_loot_table>);
		}
		std::string name() const override {
			return std::string("soil");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits(1,0,false);
		}
		BlockTraits traits()const {
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, soiltex), true, 0);
		}
	};
}
