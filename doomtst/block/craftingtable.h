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
#include "../items/crafting_table.h"
#include "../items/crafting.h"
#include "../player/player_look.h"
#include "../player/playerinventory.h"
#pragma once 

namespace items {

	struct crafting_table_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				std::string_view("crafting_table"),
				renderer::TexturePath("images\\stone.png", "StoneTexture"),
				world.get_resource<BlockRegistry>().get_id("crafting_table")
			);
		}
	};
}
namespace blocks {
	struct CraftingTableMenu:ecs::component {
		CraftingTableMenu(ecs::Constrained<ui::menu_component> menu_for,ecs::Constrained<items::container> container_for)
		:menu(menu_for),container(container_for) {

		}
		ecs::Constrained<ui::menu_component> menu;
		ecs::Constrained<items::container> container;
	};
	struct CraftingTableRecipe {
		ecs::Constrained<items::container> main_slots;
		CraftingTableRecipe(ecs::Constrained<items::container> slots) :main_slots(slots) {

		}
		void apply(ecs::obj& ent) {
			ecs::obj menu_object= ecs::spawn_emplaced<ui::MenuRecipe>(ent.world());
			menu_object.spawn_child<items::ContainerDisplayRecipe>(v2::Coord2(0, 0), main_slots.get());
			ecs::obj bg = menu_object.spawn_child<ui::ui_image_spawner>(renderer::TexturePath("images\\menutex.png", "menu_texture"), geo::Box2d(v2::Vec2(.2f, .2f), v2::Vec2(.5f, .35f)), -1);
			ecs::obj input_slots = ent.spawn_child<items::container_recipe>(v2::Coord2(2, 2));
			ent.add_component<CraftingTableMenu>(ecs::Constrained<ui::menu_component>(menu_object), ecs::Constrained<items::container>(input_slots));

			menu_object.spawn_child<items::ContainerDisplayRecipe>(v2::Coord2(8, 0), input_slots);
			ecs::Constrained<items::crafter> crafter = ent
				.spawn_child< items::CrafterRecipe>(input_slots, stn::array({ std::filesystem::path("crafting\\2x2craft.txt") }));
			menu_object.spawn_child<items::CraftingSlotDisplaySpawner>(v2::Coord2(-5, 0), crafter);
			menu_object.get_component<ui::UiEnabled>().disable();
		}
	};

	struct CraftingTableBlock:BlockType {
		void apply(ecs::obj& block) override {
			ecs::Constrained<items::container> as_slots(player::player_for(block.world()).get_component<player::inventory>().slots);
			block.apply_recipe<CraftingTableRecipe>(as_slots);
			block.add_component<items::loot_table>().add("crafting_table", 1);
		}
		std::string name() const {
			return std::string("crafting_table");
		}
	};

	struct CraftingTableClickSystem:ecs::System {
		CraftingTableClickSystem(ecs::Ecs& world) :hits(world.make_reader<player::CursorHit>()) {

		}
		void run(ecs::Ecs& world) {
			for (player::CursorHit hit:hits.read()) {
				if (world.get_resource<userinput::InputManager>().right_mouse().pressed) {
					if (hit.hit.has_component<block>()) {
						if (hit.hit.get_component<block>().is<CraftingTableBlock>()) {
							world.write_command(ui::open_menu(hit.hit.get_component<CraftingTableMenu>().menu));
						}

					}
				}
			}
		}
	private:
		ecs::EventReader<player::CursorHit> hits;
	};
	template<>
	inline constexpr BlockTraits BlockInfo<CraftingTableBlock> = BlockTraits(
		BlockMeshTraits(v3::unit_scale/2, false, craftingtableside, craftingtableside, mosstex, mosstex, craftingtableside, craftingtableside)
	);

}