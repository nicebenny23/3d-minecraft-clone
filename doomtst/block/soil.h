#pragma once

#include "block_registry.h"
#include "../items/loottable.h"
namespace items {
	
	struct DirtItem :item_type{

		std::string name() const {
			return "dirt";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(renderer::TexturePath("images\\dirt.png"),
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

	struct Seedability :ecs::component {
		Seedability(size_t amt) :seedable(amt) {

		}
		size_t seedable;
	};
	struct SoilBlock :BlockType {
		void apply(ecs::obj& block) const {

			block.apply_recipe(items::loot_table_recipe<items::soil_loot_table>);
			block.add_component<Seedability>(0);
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
		void read_from_bytes(ecs::obj block, stn::file_handle& handle)const  override {
			block.apply_recipe(items::loot_table_recipe<items::soil_loot_table>);
			block.add_component<Seedability>(stn::file_serializer<size_t>().read(handle));
		}
		void write_to_bytes(ecs::obj block, stn::file_handle& handle)const  override {

			size_t seedable=block.get_component<Seedability>().seedable;
			stn::file_serializer<double>().write(seedable, handle);
		}
	};
}
