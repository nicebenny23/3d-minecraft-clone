#include "block.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#include "../items/recipe_file.h"
#include "../items/recipe_transactions.h"
#include "../items/container_ui.h"
#include "../items/crafting.h"
#include "../player/player_look.h"
#include "../player/playerinventory.h"
#include "../player/inventory_ui.h"
#pragma once 

namespace items {

	struct crafting_table_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				std::string_view("crafting_table"),
				renderer::TexturePath("images\\craftingtabletop.png", "CraftingTableTextureTop"),
				world.get_resource<BlockRegistry>().get_id("crafting_table")
			);
		}
	};
	struct crafting_table_loot_table :items::LootTable {
		items::LootDrops drops_for(items::item_types& types) {
			return items::LootDrops({ items::loot_element(types.from_type<crafting_table_item>(),1,types) });
		}
	};
}
namespace blocks {

	struct CraftingTableMenuRecipe {
		ecs::Constrained<items::container> input_slots;

		
		void apply(ecs::obj& ent) const {
			ecs::obj menu_object= ecs::spawn_emplaced<ui::MenuRecipe>(ent.world());
			menu_object.apply_recipe(player::inventory_slots_pannel_recipe());
			ecs::obj bg = menu_object.spawn_child<ui::ui_image_spawner>(renderer::TexturePath("images\\menutex.png", "menu_texture"),
			math::Box2d(v2::Vec2(.27f, .15f), v2::Vec2(.35f, .29f))
			,0);
			ent.add_component<player::OpenMenuOnClick>(ecs::ConstrainedHandle<ui::menu_component>(menu_object));
			menu_object.spawn_child<items::ContainerDisplayRecipe>(v2::Coord2(4, 3), input_slots);
			ecs::Constrained<items::crafter> crafter = ent
				.spawn_child< items::CrafterRecipe>(input_slots.object(), stn::array({std::filesystem::path("crafting\\2x2craft.txt"), std::filesystem::path("crafting\\3x3craft.txt")}));
			menu_object.spawn_child<items::CraftingSlotDisplaySpawner>(v2::Coord2(8, 4), crafter);
			menu_object.get_component<ui::UiEnabled>().disable();
		}
	};

	struct CraftingTableBlock:BlockType {
		void apply(ecs::obj& block) const override {
			ecs::Constrained<items::container> input_slots(block.spawn_child<items::container_recipe>(v2::Coord2(3, 3)));
			block.apply_recipe(CraftingTableMenuRecipe{ .input_slots = input_slots});
			block.apply_recipe<>(items::loot_table_recipe<items::crafting_table_loot_table>);
		}
		std::string name() const {
			return std::string("crafting_table");
		}
		BlockTraits traits() const{
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, crafting_table_side, crafting_table_side, mosstex, mosstex, crafting_table_side, crafting_table_side)
			);
		}
	};


}