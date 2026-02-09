#include "../items/loottable.h"
#include "block_registry.h"
#pragma once 
namespace blocks {
	struct PlankBlock :BlockType {
		void apply(ecs::obj& block) override {

			constexpr stn::HashedString hashed("plank");
			block.add_component<items::loot_table>().add(hashed, 1);
		}
		std::string name() const{
			return std::string("plank");
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<PlankBlock> = BlockTraits(
		BlockMeshTraits(v3::unit_scale/2, false, planktex, planktex, planktex, planktex, planktex, planktex));

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