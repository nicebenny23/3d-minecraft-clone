#include "item_type.h"
#include "cursor_slot.h"
#include "Item.h"

#include "../block/craftingtable.h"
#include "../block/Chest.h"
#include "../block/plank.h"
#include "../block/air.h"
#include "../block/torch.h"
#include "../block/moss.h"
#include "../game/Core.h"
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
				"stick",
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
				renderer::TexturePath("images\\crystalbottom.png", "CrystalBottomTexture"),
				64,
				tool_traits({ .damage = 6,.pickaxe_speed = .8 }),
				0.7f,   // armor
				0.0f
			);
		}
	};

	struct crystal_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"crystal",
				renderer::TexturePath("images\\crystal.png", "CrystalTexture"),
				64,
				tool_traits({ .damage = 6,.pickaxe_speed = .8 }),
				0.0f,
				0.0f
			);
		}
	};

	struct crystal_pick_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"crystal_pick",
				renderer::TexturePath("images\\crystalpick.png", "CrystalPickaxeTexture"),
				256,
				tool_traits({ .damage = 6,.pickaxe_speed = 3 }),
				0.0f,
				12.f // pickaxe_speed from pickaxepower
			);
		}
	};

	struct crystal_top_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"crystal_top",
				renderer::TexturePath("images\\crystaltop.png", "CrystalTopTexture"),
				64,
				tool_traits({ .damage = 6,.pickaxe_speed = .8 }),
				0.0f,
				0.0f
			);
		}
	};

	struct iron_bottom_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_bottom",
				renderer::TexturePath("images\\ironbottom.png", "IronBottomTexture"),
				64,
				tool_traits({ .damage = 0,.pickaxe_speed = .8 }),
				0.0f,
				0.0f
			);
		}
	};
	struct stone_pickaxe_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"stone_pickaxe",
				renderer::TexturePath("images\\stone_pickaxe.png"),
				64,
				tool_traits({ .damage = 3,.pickaxe_speed = 6 }),
				0.0f,
				0
			);
		}
	};

	struct iron_pick_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_pick",
				renderer::TexturePath("images\\ironpick.png", "IronPickTexture"),
				64,
				tool_traits({ .damage = 3,.pickaxe_speed = 8 }),
				0.0f,
				4.9f // pickaxepower mapped to pickaxe_speed
			);
		}
	};

	struct iron_sword_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_sword",
				renderer::TexturePath("images\\ironsword.png", "IronSwordTexture"),
				64,
				tool_traits({ .damage = 6,.pickaxe_speed = .8 }),
				0.0f,
				0.0f
			);
		}
	};

	struct iron_top_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"iron_top",
				renderer::TexturePath("images\\irontop.png", "IronTopTexture"),
				64,
				tool_traits({ .damage = 0,.pickaxe_speed = .8 }),
				0.0f,
				0.0f
			);
		}
	};

	struct lava_pick_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"lava_pick",
				renderer::TexturePath("images\\pikaxe.png", "PickaxeTexture"),
				64,
				tool_traits({ .damage = 0,.pickaxe_speed = 10 }),
				0.0f,
				3.0f
			);
		}
	};

	struct rock_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"rock",
				renderer::TexturePath("images\\rock.png", "RockTexture"),
				64,
				tool_traits({ .damage = 1,.pickaxe_speed = 1}),
				0.0f,
				2.0f
			);
		}
	};

	struct sword_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"sword",
				renderer::TexturePath("images\\sword.png", "SwordTexture"),
				64,
				tool_traits({ .damage = 4,.pickaxe_speed=0}),
				0.0f,
				0.0f
			);
		}
	};

	struct final_sword_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				"final_sword",
				renderer::TexturePath("images\\finalsword.png", "FinalSwordTexture"),
				64,
				tool_traits({ .damage = 4,.pickaxe_speed = 0 }),
				0.0f,
				0.0f
			);
		}
	};


	struct register_core_items :Core::Plugin {
		void build(Core::App& app) {
			BlockRegistry& registry = app.ensure_resource<BlockRegistry>();
			item_types& item_register = app.ensure_resource<item_types>();

			registry.register_block<PlankBlock>();
			registry.register_block<MossBlock>();
			registry.register_block<TorchBlock>();
			registry.register_block<StoneBlock>();
			registry.register_block<AirBlock>();
			registry.register_block<ChestBlock>();
			registry.register_block<CraftingTableBlock>();

			item_register.register_item<items::chest_item>();
			item_register.register_item<items::crafting_table_item>();
			item_register.register_item<items::air_item>();
			item_register.register_item<items::glass_bottle_item>();
			item_register.register_item<items::iron_ore_item>();
			item_register.register_item<items::iron_item>();
			item_register.register_item<items::metal_rod_item>();
			item_register.register_item<items::moss_item>();
			item_register.register_item<items::plank_item>();
			item_register.register_item<items::stone_item>();
			item_register.register_item<items::stone_pickaxe_item>();
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