#include "item_type.h"
#include "cursor_slot.h"
#include "Item.h"
#include "crafting_table.h"
#include "../block/plank.h"
#include "../block/air.h"
#include "../block/torch.h"
#include "../block/moss.h"
#pragma once
namespace items {
 struct glass_bottle_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"glass_bottle",
				renderer::TexturePath("images\\glassbottle.png", "GlassBottleTexture")
			);
		}
	};

	struct iron_ore_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_ore",
				renderer::TexturePath("images\\ironore.png", "IronOreTexture")
			);
		}
	};

	struct iron_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron",
				renderer::TexturePath("images\\iron.png", "IronTexture")
			);
		}
	};
	struct metal_rod_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"metal_rod",
				renderer::TexturePath("images\\metalrod.png", "MetalRodTexture")
			);
		}
	};

	struct moss_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"moss",
				renderer::TexturePath("images\\mossitem.png", "MossItemTexture")
			);
		}
	}; struct crystal_bottom_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			// Uses the old initcrystalbottomitem values
			return item_traits(
				"crystal_bottom",
				item_use_type::tool,
				renderer::TexturePath("images\\crystalbottom.png", "CrystalBottomTexture"),
				0.0f,   // damage
				0.7f,   // armor
				0.0f,   // food
				0.0f    // pickaxe_speed
			);
		}
	};

	struct crystal_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"crystal",
				item_use_type::block,
				renderer::TexturePath("images\\crystal.png", "CrystalTexture"),
				0.0f,
				0.0f,
				0.0f,
				0.0f
			);
		}
	};

	struct crystal_pick_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"crystal_pick",
				item_use_type::tool,
				renderer::TexturePath("images\\crystalpick.png", "CrystalPickaxeTexture"),
				0.0f,
				0.0f,
				0.0f,
				7.0f // pickaxe_speed from pickaxepower
			);
		}
	};

	struct crystal_top_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"crystal_top",
				item_use_type::tool,
				renderer::TexturePath("images\\crystaltop.png", "CrystalTopTexture"),
				0.0f,  // damage
				0.7f,  // armor
				0.0f,
				0.0f
			);
		}
	};

	struct iron_bottom_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_bottom",
				item_use_type::tool,
				renderer::TexturePath("images\\ironbottom.png", "IronBottomTexture"),
				0.0f,
				0.8f,
				0.0f,
				0.0f
			);
		}
	};

	struct iron_pick_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_pick",
				item_use_type::tool,
				renderer::TexturePath("images\\ironpick.png", "IronPickTexture"),
				0.0f,
				0.0f,
				0.0f,
				4.9f // pickaxepower mapped to pickaxe_speed
			);
		}
	};

	struct iron_sword_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_sword",
				item_use_type::tool,
				renderer::TexturePath("images\\ironsword.png", "IronSwordTexture"),
				6.0f,  // damage
				0.0f,  // armor
				0.0f,
				0.0f
			);
		}
	};

	struct iron_top_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_top",
				item_use_type::tool,
				renderer::TexturePath("images\\irontop.png", "IronTopTexture"),
				0.0f,
				0.8f,
				0.0f,
				0.0f
			);
		}
	};

	struct lava_pick_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"lava_pick",
				item_use_type::tool,
				renderer::TexturePath("images\\pikaxe.png", "PickaxeTexture"),
				0.0f,
				0.0f,
				0.0f,
				3.0f
			);
		}
	};

	struct rock_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"rock",
				item_use_type::tool,
				renderer::TexturePath("images\\rock.png", "RockTexture"),
				0.0f,
				0.0f,
				0.0f,
				2.0f
			);
		}
	};

	struct sword_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"sword",
				item_use_type::tool,
				renderer::TexturePath("images\\sword.png", "SwordTexture"),
				4.0f,
				0.0f,
				0.0f,
				0.0f
			);
		}
	};

	struct final_sword_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"final_sword",
				item_use_type::tool,
				renderer::TexturePath("images\\finalsword.png", "FinalSwordTexture"),
				10.0f,
				0.0f,
				0.0f,
				0.0f
			);
		}
	};


	struct register_core_items :Core::Plugin {
		void build(Core::App& app) {
			BlockRegistry& registry = app.ensure_resource<BlockRegistry>();
			registry.insert_id<PlankBlock>();
			registry.insert_id<MossBlock>();
			registry.insert_id<TorchBlock>();
			registry.insert_id<StoneBlock>();
			registry.insert_id<AirBlock>();

			app.emplace_system< ItemClear>();
			item_types& item_register = app.ensure_resource<item_types>();
			app.emplace_system< run_crafts>();
			app.emplace_system< cursor_crafter>();
			app.emplace_system<SyncDisplayIcon>();
			item_register.register_item<items::air_item>();
			item_register.register_item<items::glass_bottle_item>();
			item_register.register_item<items::iron_ore_item>();
			item_register.register_item<items::iron_item>();
			item_register.register_item<items::metal_rod_item>();
			item_register.register_item<items::moss_item>();
			item_register.register_item<items::plank_item>();
			item_register.register_item<items::stone_item>();
			// Crystal items
			item_register.register_item<items::crystal_bottom_item>();
			item_register.register_item<items::crystal_item>();
			item_register.register_item<items::crystal_pick_item>();
			item_register.register_item<items::crystal_top_item>();

			// Iron items
			item_register.register_item<items::iron_bottom_item>();
			item_register.register_item<items::iron_pick_item>();
			item_register.register_item<items::iron_sword_item>();
			item_register.register_item<items::iron_top_item>();

			// Tools
			item_register.register_item<items::lava_pick_item>();
			item_register.register_item<items::rock_item>();
			item_register.register_item<items::sword_item>();

			item_register.register_item<torch_item>();
			app.insert_plugin<items::CursorContainerPlugin>();
			item_register.register_item<items::final_sword_item>();
		}
	};

}