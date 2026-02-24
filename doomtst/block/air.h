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
namespace items {
	struct air_item :item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(std::string_view("air"), renderer::TexturePath("images\\airtex.png", "AirTexture"),world.get_resource<BlockRegistry>().get_id<blocks::AirBlock>());
		}
	};
}