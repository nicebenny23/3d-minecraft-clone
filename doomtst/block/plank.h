
#include "block.h"
#include "../items/loottable.h"
#include "block_registry.h"
#pragma once 
namespace blocks {
	struct PlankBlock :BlockType {
		void apply(ecs::obj& block) override {

			constexpr stn::HashedString hashed("moss");
			block.add_component<items::loot_table>().add(hashed, 1);
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<PlankBlock> = BlockTraits(
		BlockMeshTraits(v3::unit_scale/2, false, planktex, planktex, planktex, planktex, planktex, planktex));

}