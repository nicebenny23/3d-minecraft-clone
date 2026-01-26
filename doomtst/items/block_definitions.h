#include "item_type.h"
#include "cursor_slot.h"
#include "Item.h"
#include "crafting_table.h"
#pragma once
namespace items {
	struct air_item :item_type {
		item_traits traits() const {
			return item_traits("air", renderer::TexturePath("images\\airtex.png", "AirTexture"), minecraftair);
		}
	};
	struct altar_item :item_type {
		item_traits traits() const {
			return item_traits("altar", renderer::TexturePath("images\\crystalaltarside.png", "CrystalAltarSideTexture"), minecraftaltar);
		}
	};
	struct crafting_table_item :item_type {
		item_traits traits() const {
			return item_traits("crafting_table", renderer::TexturePath("images\\craftingtabletop.png", "CraftingTableTopTexture"), minecraftcraftingtable);
		}
	}; struct glass_bottle_item : item_type {
		item_traits traits() const {
			return item_traits(
				"glass_bottle",
				renderer::TexturePath("images\\glassbottle.png", "GlassBottleTexture"),
				stn::None
			);
		}
	};

	struct iron_ore_item : item_type {
		item_traits traits() const {
			return item_traits(
				"iron_ore",
				renderer::TexturePath("images\\ironore.png", "IronOreTexture"),
				blockname::id::minecraftironore
			);
		}
	};

	struct iron_item : item_type {
		item_traits traits() const {
			return item_traits(
				"iron",
				renderer::TexturePath("images\\iron.png", "IronTexture"),
				stn::None
			);
		}
	};

	struct metal_rod_item : item_type {
		item_traits traits() const {
			return item_traits(
				"metal_rod",
				renderer::TexturePath("images\\metalrod.png", "MetalRodTexture"),
				stn::None
			);
		}
	};

	struct moss_item : item_type {
		item_traits traits() const {
			return item_traits(
				"moss",
				renderer::TexturePath("images\\mossitem.png", "MossItemTexture"),
				stn::None
			);
		}
	}; struct crystal_bottom_item : item_type {
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
		item_traits traits() const {
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
	struct plank_item : item_type {
		item_traits traits() const {
			return item_traits(
				"plank",
				renderer::TexturePath("images\\treestoneblock.png", "PlankTexture"),
				blockname::minecrafttreestone
			);
		}
	};

	struct stone_item : item_type {
		item_traits traits() const {
			return item_traits(
				"stone",
				renderer::TexturePath("images\\stone.png", "StoneTexture"),
				blockname::minecraftstone
			);
		}
	};
	struct register_core_items :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system< ItemClear>();
			item_type_register& item_register = app.ensure_resource<item_type_register>();
			app.emplace_system< run_crafts>();
			app.emplace_system<SyncDisplayIcon>();
			item_register.register_item<items::air_item>();
			item_register.register_item<items::altar_item>();
			item_register.register_item<items::crafting_table_item>();
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

			app.insert_plugin<items::CursorContainerPlugin>();
			item_register.register_item<items::final_sword_item>();
		}
	};

}