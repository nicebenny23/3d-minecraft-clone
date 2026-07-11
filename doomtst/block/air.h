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
		BlockMeshTraits traits(BlockTextureRegistry& registry) const{
			return BlockMeshTraits(v3::Scale3::from_scale(.5f), true, registry.get_texture("images\\stone.png"),true);
		}
		bool is_solid() const override {
			return false;
		}
	};

}