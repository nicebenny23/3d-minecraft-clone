#include "playerinventory.h"
#include "../game/ecs/filtered_object.h"
#include "../items/menu.h"
#include "../items/crafting.h"
#pragma once
namespace player {

	struct inventory_ui :ecs::component {
		inventory_ui(ecs::Constrained<ui::MenuComponent> inventory_menu) :menu(inventory_menu) {

		}

		ecs::Constrained<ui::MenuComponent> menu;
	};
	struct InventoryUiSystem : ecs::System {
		void run(ecs::Ecs& world) {

			userinput::InputManager& input = world.get_resource<userinput::InputManager>();
			auto view = ecs::View< inventory, inventory_ui>(world);
			for (auto [player_inv, ui_menu] : view) {
				if (input.key('1').pressed) {

					player_inv.selected_ind = v2::UVec2(0, 0);
				}
				if (input.key('2').pressed) {

					player_inv.selected_ind =v2::UVec2(1, 0);
				}
				if (input.key('3').pressed) {

					player_inv.selected_ind = v2::UVec2(2, 0);
				}if (input.key('4').pressed) {

					player_inv.selected_ind = v2::UVec2(3, 0);
				}
				if (input.key('5').pressed) {

					player_inv.selected_ind = v2::UVec2(4, 0);
				}
				if (input.key('6').pressed) {
					player_inv.selected_ind = v2::UVec2(5, 0);

				}
				if (input.key('e').pressed) {
					world.write_command(ui::open_menu(ui_menu.menu));
				}
			}


		}


	};


	struct inventory_slots_pannel_recipe {
		void apply(ecs::obj& ent) const{
			ent.spawn_child<items::ContainerDisplayRecipe>(v2::Coord2(0, -1), player_for(ent.world()).get_component<inventory>().slots);
		}
	};
	struct inventory_menu_recipe {

		ecs::Constrained<items::container> input;
		ecs::Constrained<items::container> main_slots;
		inventory_menu_recipe(ecs::Constrained<items::container> slots, ecs::Constrained<items::container> input_slots) :main_slots(slots),input(input_slots) {

		}
		void apply(ecs::obj& ent) const{
			ui::MenuRecipe().apply(ent);
			inventory_slots_pannel_recipe().apply(ent);
			ecs::obj bg = ent.spawn_child<ui::UiImageSpawner>(renderer::TexturePath("images\\menutex.png"), geo::Box2d(v2::Vec2(.24f, .15f)/2.0, v2::Vec2(.33f, .25f)), 0);
			
			ent.spawn_child<items::ContainerDisplayRecipe>(v2::Coord2(4, 3), input);
ecs::Constrained<items::crafter> crafter = ent
				.spawn_child< items::CrafterRecipe>(input.object(), stn::array({std::filesystem::path("crafting\\2x2craft.txt")}));			
			ent.spawn_child<items::CraftingSlotDisplaySpawner>(v2::Coord2(7, 3), crafter);
			ent.get_component<ui::UiEnabled>().disable();
		}
	};



	struct PlayerInventoryRecipe {
		void apply(ecs::obj& object) const{
			ecs::Constrained<items::container>  slots = object.spawn_child<items::container_recipe>(ui::TableBounds(6, 2));
			ecs::Constrained<items::container> hotbar = ecs::spawn(object.world(), items::container_recipe(ui::TableBounds(6, 1)));
			ecs::Constrained<items::ContainerDisplay>	hotbar_display = ecs::spawn(object.world(), items::ContainerDisplayRecipe(v2::Coord2(0, -7), hotbar));
			stn::array<std::string> items({ "plank"});
			object.add_component<inventory>(slots, hotbar, hotbar_display).
				givestartitems(items);
			ecs::obj input_slots = ecs::spawn(object.world(),items::container_recipe(ui::TableBounds(2, 2)));
			ecs::Constrained<ui::MenuComponent> inventory_menu = ecs::spawn(object.world(), inventory_menu_recipe(slots,input_slots));
			object.add_component<inventory_ui>(inventory_menu);
		}
	};

	struct LoadHotbarSlots :ecs::System {
		void run(ecs::Ecs& world) {
			//replace

			for (auto [inventory_slot] : ecs::View< inventory>(world)) {
				//we need some sort of wa
				stn::Option<v2::UVec2> index = inventory_slot.selected_ind;
				items::ContainerDisplay& hotbar_container = inventory_slot.hotbar_display.get_component<items::ContainerDisplay>();
				if (index) {
					hotbar_container[index.unwrap()].get_component<items::ItemSlotDecal>().set_decal(
						renderer::TexturePath("images\\importantblockholder.png")
					);
				}

			}

		}
	};

	struct PlayerInventoryPlugin {
		void operator()(core::App& world) {
			player::player_for(world.Ecs).apply_recipe(PlayerInventoryRecipe());
			world.emplace_system< InventoryUiSystem>();
			world.emplace_system<player::LoadHotbarSlots>();
			world.emplace_system< items::ItemClear>();
			world.emplace_system< items::run_crafts>();
			world.emplace_system< items::cursor_crafter>();
			world.emplace_system<items::SyncDisplayIcon>();
		}
	};
}