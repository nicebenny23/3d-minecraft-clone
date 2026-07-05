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
		BlockTraits traits(BlockTextureRegistry& registry) const{
			return BlockTraits(
				BlockMeshTraits(v3::Scale3(.5f), true, registry.get_texture("images\\stone.png"), true)
				, non_solid_block);
		}
	};

}