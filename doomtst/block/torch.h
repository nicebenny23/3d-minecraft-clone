#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace blocks {
	struct TorchBlock :BlockType {
		TorchBlock() = default;
		void apply(ecs::obj& block) const override{
			block.add_component<items::loot_table>().add("torch", 1);
		}
		std::string name() const {
			return std::string("torch");
		}
		BlockTraits traits() const {
			return BlockTraits(
				BlockMeshTraits(v3::Scale3(1 / 16.f, .375, 1 / 16.f), true, torchtex, torchtex, torchtex, torchtex, torchtex, torchtex)
				, true, 14);
		}
	};

}
namespace items {
	struct torch_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"torch",
				renderer::TexturePath("images\\torch.png", "TorchTexture"),
				world.get_resource<BlockRegistry>().get_id<blocks::TorchBlock>()
			);
		}
	};


}