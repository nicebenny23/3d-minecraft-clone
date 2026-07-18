#include "../items/loottable.h"
#include "block_registry.h"
#pragma once
namespace blocks {
	struct WaterBlock :BlockType {
		WaterBlock() {

		}
		std::string name() const {
			return std::string("water");
		}
		void apply(ecs::obj& block) const override {
		}
		stn::Option<SolidBlockTraits> solid_traits_for() const override {
			return stn::None;
		}
		BlockMeshTraits traits(BlockTextureRegistry& registry) const {
			block_texture plank_texture = registry.get_texture("images\\water.png");
			return BlockMeshTraits(v3::unit_scale,true, plank_texture);
		}
	};
}