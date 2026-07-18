#pragma once

#include "block_registry.h"
#include "../items/loottable.h"
namespace items {

	struct CrystalItem : item_type {
		std::string name() const{
			return "crystal";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits{ .image_path{renderer::TexturePath("images\\crystal.png")} };
		}
	};
	struct crystal_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element(types.insert<CrystalItem>(),1,types) });
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
		stn::Option<SolidBlockTraits> solid_traits_for() const override {
			return SolidBlockTraits(4,2);
		}
		BlockMeshTraits traits(BlockTextureRegistry& registry)const {
			return BlockMeshTraits(v3::unit_scale, false, registry.get_texture("images\\crystalore.png"));
		}
		size_t emmited_light() const override {
			return 4;
		}
	};
}