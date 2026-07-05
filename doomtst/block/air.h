#include "block_registry.h"
#include "../items/item_type.h"
#pragma once 
namespace blocks {
	struct AirBlock :BlockType  {
		void apply(ecs::obj& block) const override {
		}
		std::string name() const {
			return std::string("air");
		}
		BlockTraits traits() const{
			return BlockTraits(
				BlockMeshTraits(v3::Scale3(.5f), true, stonetex, mosstex, mosstex, mosstex, mosstex, mosstex,true)
				, non_solid_block);
		}
	};

}