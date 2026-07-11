#include "block_registry.h"
#include "../items/loottable.h"
#pragma once 
namespace blocks {

	struct torch_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element("torch",1,types) });
		}
	};
	struct TorchBlock :BlockType {
		TorchBlock() = default;
		void apply(ecs::obj& block) const override{
			block.apply_recipe(items::loot_table_recipe<torch_loot_table>);
		}
		std::string name() const {
			return std::string("torch");
		}
		BlockMeshTraits traits(BlockTextureRegistry& texture) const {
			block_texture torchtex= texture.get_texture("images\\torch.png");
			block_texture torchtoptex = texture.get_texture("images\\torchtop.png");
			return BlockMeshTraits(v3::Scale3(1/8.f,.75f, 1/ 8.f), true, torchtex, torchtex, torchtoptex, torchtex, torchtex, torchtex);
		}
		size_t emmited_light() const override {
			return 14;
		}
	};

}
namespace items {
	struct TorchItem : item_type {
		std::string name() const {
			return "torch";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(
				renderer::TexturePath("images\\torch.png"),
				world.get_resource<BlockRegistry>().get_id<blocks::TorchBlock>()
			);
		}
	};


}