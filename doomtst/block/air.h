#include "block.h"
#include "block_registry.h"
#pragma once 
namespace blocks {
	struct AirBlock :BlockType  {
		void apply(ecs::obj& block) override {
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<AirBlock> = BlockTraits(
		BlockMeshTraits(v3::Scale3(0), true, stonetex, mosstex, mosstex, mosstex, mosstex, mosstex)
	,non_solid_block);

}