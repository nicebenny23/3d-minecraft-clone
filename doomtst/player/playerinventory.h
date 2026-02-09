#include "../items/container_transactions.h"
#include "../game/ecs/game_object.h"
#include "../util/userinput.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../items/container_ui.h"
#include "../items/crafting.h"
#pragma once 
namespace player {
	struct main_slots_component :ecs::component {
		main_slots_component(ecs::obj slots) :main_item_slots(slots) {

		}
		ecs::obj main_item_slots;
		
	};
	struct SlotPannelComponent :ecs::component {

		SlotPannelComponent(main_slots_component& slot) {
		}
		ecs::obj main_item_slots;
	};
	struct SlotPannelRecipe :ecs::Recipe {
		ecs::obj main_item_slots;
		v2::Coord2 location;
		SlotPannelRecipe(main_slots_component& slots,v2::Coord2 place):main_item_slots(slots.owner()), location(place){
		}
		void apply(ecs::obj& entity) {
			ecs::obj slots=ecs::spawn(entity.world(), items::ContainerDisplayRecipe(v2::Coord2(1, 2), main_item_slots));
			entity.add_component<SlotPannelComponent>(main_item_slots.get_component< main_slots_component>();
		}
	};
	struct inventory_menu_recipe :ecs::Recipe {
		inventory_menu_recipe() {

		}

		void apply(ecs::obj& ent) {
			ent.add_component<ui::menu_component>();
			ecs::obj main_slots = ent.spawn_child<items::container_recipe>( v2::Coord2(6, 5));
			ui::MenuRecipe().apply(ent);
			ecs::obj bg= ent.spawn_child<ui::ui_image_spawner>(renderer::TexturePath("images\\menutex.png","menu_texture"), geo::Box2d(v2::Vec2(.2f,.2f), v2::Vec2(.5f, .35f)), -1);
			
			ecs::spawn(ent.world(), items::ContainerDisplayRecipe(v2::Coord2(-12,-12), main_slots));

			//items::CrafterRecipe(input, display, std::filesystem::path("crafting\\2x2craft.txt")).apply(ent);
			ent.get_component<ui::UiEnabled>().disable();
		}
	};
	struct HotbarSlot :ecs::component {

	};


	struct inventory : ecs::component {
		void givestartitems(stn::array<std::string>& items) {
			for (const std::string& item : items) {
				items::item_id id = world().get_resource<items::item_types>().from_name(item);
				items::item_entry entry = items::item_entry(id, 10, world().insert_resource<items::item_types>());
				stn::Option<items::AddToSlotPlan> plan = items::give_container_entry(entry, hotbar.get_component < items::container>());
				if (plan) {
					plan.unwrap().apply(world());
				}
			}
		}
		inventory() {


		}

		ecs::obj input;
		ecs::obj inventory_object;
		ecs::obj hotbar;
		stn::Option<items::container_index> selected_ind;
		stn::Option<items::item_stack&> selected() {
			if (selected_ind == stn::None) {
				return stn::None;
			}
			return
				hotbar.get_component<items::container>()[selected_ind.unwrap()]
				.get_component<items::ElementSlot>()
				.stack();
		}
		stn::Option<ecs::obj> selected_object() {
			if (selected_ind == stn::None) {
				return stn::None;
			}
			return hotbar.get_component<items::container>()[selected_ind.unwrap()]
				.get_component<items::ElementSlot>()
				.element();
		}
		void start() {
			inventory_object =ecs::spawn(world(),inventory_menu_recipe());
			hotbar = ecs::spawn(world(), items::container_recipe(v2::Coord2(6,1)));
			ecs::spawn(world(), items::ContainerDisplayRecipe(v2::Coord2(0,0), hotbar));
			for (ecs::object_handle& handle : hotbar.get_component<items::container>()) {
				handle.add_component< HotbarSlot>();
			}
			stn::array<std::string> items({ "plank","torch","stone"});
			givestartitems(items);
		}
		void update() {

			userinput::InputManager& input = world().ensure_resource<userinput::InputManager>();
			if (input.getKey('e').pressed) {
				world().write_command(ui::open_menu(inventory_object.get_component<ui::menu_component>()));
			}
			if (input.getKey('1').pressed) {

				selected_ind = items::container_index(0, 0);
			}
			if (input.getKey('2').pressed) {

				selected_ind = items::container_index(1, 0);
			}
			if (input.getKey('3').pressed) {

				selected_ind = items::container_index(2, 0);
			}if (input.getKey('4').pressed) {

				selected_ind = items::container_index(3, 0);
			}
			if (input.getKey('5').pressed) {

				selected_ind = items::container_index(4, 0);
			}
			if (input.getKey('6').pressed) {
				selected_ind = items::container_index(5, 0);

			}
		}

	};
	struct HotbarCommands :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [inventory_slot] : ecs::View<inventory>(world)) {
				stn::Option<items::container_index> index = inventory_slot.selected_ind;
				items::container& hotbar_container = inventory_slot.hotbar.get_component<items::container>();
				for (ecs::object_handle& handle : hotbar_container) {
					handle.get_component<items::ItemSlotDecal>().reset_decal();
				}
				if (index) {
					hotbar_container[index.unwrap()].get_component<items::ItemSlotDecal>().set_decal(
						renderer::TexturePath("images\\importantblockholder.png", "important_block_holder")
					);
				}
			}
		}
	};

	struct LoadHotbarSlots :ecs::System {
		void run(ecs::Ecs& world) {
			for (auto&& [inventory_slot] : ecs::View<inventory>(world)) {
				stn::Option<items::container_index> index = inventory_slot.selected_ind;
				items::container& hotbar_container = inventory_slot.hotbar.get_component<items::container>();
				for (ecs::object_handle& handle : hotbar_container) {
					//handle.get_component<items::>().reset_decal();
				}
				if (index) {
					/*hotbar_container[index.unwrap()].get_component<items::ItemSlotDecal>().set_decal(
						renderer::TexturePath("images\\importantblockholder.png", "important_block_holder")
					);
					*/
				}
			}
		}
	};


}

// !playerinventory_H
