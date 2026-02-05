#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace blocks {
	struct MossBlock :BlockType  {
		void apply(ecs::obj& block) override {
			block.add_component<items::loot_table>().add("moss", 1);
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<MossBlock> = BlockTraits(
		BlockMeshTraits(v3::unit_scale,false,mosstex,mosstex,mosstex,mosstex,mosstex,mosstex)
	);

}