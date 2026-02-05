
#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#include "../items/block_definitions.h"
#pragma once 
namespace blocks {
	struct StoneBlock :BlockType {
		void apply(ecs::obj& block) override {

			block.add_component<items::loot_table>().add<items::stone_item>(1);
		}
		SolidBlockTraits mining_traits() const override {
			return SolidBlockTraits{ .time_to_mine = 3 };
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<StoneBlock> = BlockTraits(
		BlockMeshTraits(v3::unit_scale/2, false, stonetex, stonetex, stonetex, stonetex, stonetex, stonetex));

}