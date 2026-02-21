#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace blocks {
	struct MossBlock :BlockType  {
		void apply(ecs::obj& block) const override {
			block.add_component<items::loot_table>().add("moss", 1);
		}
		std::string name() const {
			return std::string("moss");
		}
		BlockTraits traits() const {
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, mosstex, mosstex, mosstex, mosstex, mosstex, mosstex)
			);
		}
	};

}