
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
}
namespace blocks {
	struct StoneBlock :BlockType {
		void apply(ecs::obj& block) {
			block.add_component<items::loot_table>().add<items::stone_item>(1);
		}
		std::string name() const override {
			return std::string("stone");
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits{ .time_to_mine = 3 };
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<StoneBlock> = BlockTraits(
		BlockMeshTraits(v3::unit_scale / 2, false, stonetex, stonetex, stonetex, stonetex, stonetex, stonetex));

}