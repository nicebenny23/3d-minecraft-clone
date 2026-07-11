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
	
	struct MossPackItem : item_type {
		std::string name() const {
			return "moss_pack";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits{ .image_path{"images\\moss_pack.png" },.food = 3 };
		}
	};

	struct MossItem : item_type {
		std::string name() const {
			return "moss";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits{ .image_path=renderer::TexturePath("images\\mossitem.png"),.fertilizer = 1};
		}
	};

	struct CrystalPickItem : item_type {
		std::string name() const {
			return "crystal_pickaxe";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 3, .pickaxe_speed = 4 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::dur_item(
				renderer::TexturePath("images\\crystalpick.png"),
				256
			);
		}
	};

	struct StonePickaxeItem : item_type {
		std::string name() const {
			return "stone_pickaxe";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 2, .pickaxe_speed = 3 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::dur_item(
				renderer::TexturePath("images\\stone_pickaxe.png"),
				64
			);
		}
	};

	struct SwordItem : item_type {
		std::string name() const {
			return "stone_sword";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits{
				.damage = 3, .pickaxe_speed = 0
			};
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::dur_item(
				renderer::TexturePath("images\\sword.png"), 16);
		}
	};

	struct CrystalSwordItem : item_type {
		std::string name() const {
			return "crystal_sword";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 4, .pickaxe_speed = 0 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::dur_item(
				renderer::TexturePath("images\\crystal_dagger.png"),
				64
			);
		}
	};

	struct RopeItem : item_type {
		std::string name() const {
			return "final_sword";
		}
		stn::Option<tool_traits> tool_info() const {
			return tool_traits({ .damage = 4, .pickaxe_speed = 0 });
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::dur_item(
				renderer::TexturePath("images\\finalsword.png"),
				64
			);
		}
	};


	inline void register_core_items(core::App& app) {
		app.ensure_resource<loot_tables>();
		using namespace items;
		BlockRegistry& registry = app.ensure_resource<BlockRegistry>();
		ItemTypes& item_register = app.ensure_resource<ItemTypes>();

		registry.insert<LogBlock>();
		registry.insert<PlankBlock>();
		registry.insert<SeedBlock>();
		registry.insert<TorchBlock>();
		registry.insert<StoneBlock>();
		item_register.insert<TorchItem>();
		registry.insert<StoneBrickBlock>();
		item_register.insert<StoneBricksItem>();

		item_register.insert<StoneItem>();

		registry.insert<CrystalBlock>();
		item_register.insert<CrystalItem>();


		registry.insert<GlassBlock>();
		item_register.insert<GlassItem>();

		registry.insert<AirBlock>();
		registry.insert<ChestBlock>();

		registry.insert<SoilBlock>();

		item_register.insert<DirtItem>();
		registry.insert<CraftingTableBlock>();
		item_register.insert<ChestItem>();
		item_register.insert<CraftingTableItem>();
		item_register.insert<SeedItem>();
		item_register.insert<MossItem>();
		item_register.insert<MossPackItem >();
		item_register.insert<PlankItem>();
		item_register.insert<StonePickaxeItem>();
		// Crystal items
		item_register.insert<CrystalSwordItem>();
		item_register.insert<CrystalPickItem>();
		// Iron items
		item_register.insert<MulchItem>();
		item_register.insert<SwordItem>();
		app.emplace_system<SeedGrower>();
		app.insert_plugin(cursor_container_plugin);

		app.insert_plugin(soil_plugin);
		item_register.insert<RopeItem>();
	}
}