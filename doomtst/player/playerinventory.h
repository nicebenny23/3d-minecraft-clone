#include "../items/container_transactions.h"
#include "../game/ecs/game_object.h"
#include "../util/userinput.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../items/crafting.h"
#pragma once 
struct inventory_menu_component :ecs::Recipe {
	inventory_menu_component() {

	}
	void apply(ecs::obj& ent) {
		ent.add_component<ui::menu_component>();
		items::container_recipe(v2::Coord2(2, 3), v2::Coord2(6, 5)).apply(ent);
	}
};
struct HotbarSlot:ecs::component{

};


struct inventory : ecs::component {
	void givestartitems(std::string items) {
		items::item_id id = world().get_resource<items::item_types>().unwrap().from_name(items);
		items::item_entry entry = items::item_entry(id, 10, world().ensure_resource<items::item_types>());
		stn::Option<items::AddToSlotPlan> plan = items::give_container_entry(entry, hotbar.get_component < items::container>());
		if (plan) {
			plan.unwrap().apply(world());
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

		hotbar = world().spawn_empty();
		ecs::obj input = ecs::spawn(world(), items::container_recipe(v2::Coord2(0, 6), v2::Coord2(2, 2)));

		inventory_object = ecs::spawn(world(), inventory_menu_component());
		hotbar = ecs::spawn(world(), items::container_recipe(v2::Coord2(-1, -3), v2::Coord2(6, 1)));
		for (ecs::object_handle& handle : hotbar.get_component<items::container>()) {
			handle.add_component< HotbarSlot>();
		}
		ecs::obj display = ecs::spawn(world(), items::DisplaySlot(v2::Coord2(4, 8)));

		ecs::obj own = owner();
		items::make_crafting_table(input, display).apply(own);
		givestartitems("plank");
	}
	void update() {

		userinput::InputManager& input=world().ensure_resource<userinput::InputManager>();
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
namespace player {
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

}


// !playerinventory_H
