#include "item_type.h"
#include "../block/water.h"
#pragma once
namespace item {

	struct EmptyBottleItem :items::item_type {
		std::string name() const {
			return "bottle";
		}
		items::item_traits traits(const ecs::Ecs& world) const {
			return items::item_traits{.image_path=renderer::TexturePath("images\\bottle.png")};
		}
	};

	struct WaterBottleItem :items::item_type {
		std::string name() const {
			return "water_bottle";
		}
		items::item_traits traits(const ecs::Ecs& world) const {
			return items::item_traits::block_item(renderer::TexturePath("images\\water_bottle.png"),
				world.get_resource<blocks::BlockRegistry>().get_id<blocks::WaterBlock>());
		}
	};
}