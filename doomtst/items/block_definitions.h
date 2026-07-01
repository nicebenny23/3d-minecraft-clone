#include "item_type.h"
#include "cursor_slot.h"
#include "Item.h"

#include "../block/craftingtable.h"
#include "../block/Chest.h"
#include "../block/plank.h"
#include "../block/crystal.h"
#include "../block/soil.h"
#include "../block/air.h"
#include "../block/torch.h"
#include "../block/moss.h"
#include "../block/glass.h"
#include "../game/Core.h"

#pragma once
namespace items {
	struct glass_bottle_item : item_type {
		std::string name() const {
			return "glass_bottle";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\glassbottle.png")
			);
		}
	};

	struct iron_ore_item : item_type {
		std::string name() const {
			return "iron_ore";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\ironore.png")
			);
		}
	};

	struct iron_item : item_type {
		std::string name() const {
			return "iron";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\iron.png")
			);
		}
	};

	struct stick_item : item_type {
		std::string name() const {
			return "stick";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\metalrod.png")
			);
		}
	};

	struct slime_item : item_type {
		std::string name() const {
			return "slime";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\slime.png")
			);
		}
	};

	struct moss_pack_item : item_type {
		std::string name() const {
			return "moss_pack";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::food_item(
				renderer::TexturePath("images\\moss_pack.png"), 3);
		}
	};

	struct moss_item : item_type {
		std::string name() const {
			return "moss";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(renderer::TexturePath("images\\mossitem.png"));
		}
	};

	struct crystal_pick_item : item_type {
		std::string name() const {
			return "crystal_pickaxe";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 3, .pickaxe_speed = 4 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\crystalpick.png"),
				256
			);
		}
	};

	struct stone_pickaxe_item : item_type {
		std::string name() const {
			return "stone_pickaxe";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 2, .pickaxe_speed = 3 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\stone_pickaxe.png"),
				64
			);
		}
	};

	struct iron_pick_item : item_type {
		std::string name() const {
			return "iron_pick";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 3, .pickaxe_speed = 8 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\ironpick.png"),
				64
			);
		}
	};

	struct iron_sword_item : item_type {
		std::string name() const {
			return "iron_sword";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 6, .pickaxe_speed = .8 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\ironsword.png"),
				64
			);
		}
	};


	struct rock_item : item_type {
		std::string name() const {
			return "rock";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 1, .pickaxe_speed = 1 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\rock.png"),
				64
			);
		}
	};

	struct sword_item : item_type {
		std::string name() const {
			return "stone_sword";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits{
				.damage = 3, .pickaxe_speed = 0
			};
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\sword.png"), 16);
		}
	};

	struct crystal_sword_item : item_type {
		std::string name() const {
			return "crystal_sword";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 4, .pickaxe_speed = 0 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\crystal_dagger.png"),
				64
			);
		}
	};

	struct rope_item : item_type {
		std::string name() const {
			return "final_sword";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 4, .pickaxe_speed = 0 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				renderer::TexturePath("images\\finalsword.png"),
				64
			);
		}
	};


	inline void register_core_items(core::App& app) {
		app.ensure_resource<loot_tables>();
		BlockRegistry& registry = app.ensure_resource<BlockRegistry>();
		ItemTypes& item_register = app.ensure_resource<ItemTypes>();

		registry.insert<LogBlock>();
		registry.insert<PlankBlock>();
		registry.insert<SeedBlock>();
		registry.insert<TorchBlock>();
		registry.insert<StoneBlock>();

		registry.insert<StoneBrickBlock>();
		item_register.insert<stone_bricks_item>();

		item_register.insert<items::stone_item>();

		registry.insert<CrystalBlock>();
		item_register.insert<items::crystal_item>();


		registry.insert<GlassBlock>();
		item_register.insert<items::glass_item>();

		registry.insert<AirBlock>();
		registry.insert<ChestBlock>();

		registry.insert<SoilBlock>();

		item_register.insert<items::DirtItem>();
		registry.insert<CraftingTableBlock>();
		item_register.insert<items::slime_item>();

		item_register.insert<items::torch_item>();
		item_register.insert<items::chest_item>();
		item_register.insert<items::crafting_table_item>();
		item_register.insert<items::air_item>();
		item_register.insert<items::glass_bottle_item>();
		item_register.insert<items::iron_ore_item>();
		item_register.insert<items::iron_item>();
		item_register.insert<items::stick_item>();
		item_register.insert<items::seed_item>();
		item_register.insert<items::moss_item>();
		item_register.insert<items::moss_pack_item >();
		item_register.insert<items::plank_item>();
		item_register.insert<items::stone_pickaxe_item>();
		// Crystal items
		item_register.insert<items::crystal_sword_item>();
		item_register.insert<items::crystal_pick_item>();
		// Iron items
		item_register.insert<items::iron_pick_item>();
		item_register.insert<items::iron_sword_item>();

		// Tools
		item_register.insert<items::rock_item>();
		item_register.insert<items::sword_item>();
		app.emplace_system<SeedGrower>();
		app.insert_plugin(items::CursorContainerPlugin());
		item_register.insert<items::rope_item>();
	}
}