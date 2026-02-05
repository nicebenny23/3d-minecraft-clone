#include "block.h"
#include "../items/loottable.h"
#pragma once 
namespace blocks {
	struct TorchBlock :BlockType {
		TorchBlock() = default;
		void apply(ecs::obj& block) override{
			block.add_component<items::loot_table>().add("torch", 1);
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<TorchBlock> = BlockTraits(
		BlockMeshTraits(v3::Scale3(1 / 16.f, .375, 1 / 16.f), true, torchtex, torchtex, torchtex, torchtex, torchtex, torchtex)
	,true,14);

}