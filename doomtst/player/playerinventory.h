#include "../items/container_transactions.h"
#include "../game/ecs/game_object.h"
#include "../util/userinput.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#pragma once 
struct inventory_menu_component :ecs::Recipe {
	inventory_menu_component() {

	}
	void apply(ecs::obj& ent) {
		ent.add_component<ui::menu_component>();
		items::container_recipe(v2::Coord2(2, 3), v2::Coord2(6, 5)).apply(ent);
	}
};


struct inventory : ecs::component {
	void givestartitems(std::string items);
	inventory() {


	}

	ecs::obj inventory_object;
	ecs::obj hotbar;
	stn::Option<items::container_index> selected_ind;
	stn::Option<items::item_stack&> selected() {
		if (selected_ind==stn::None) {
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

		inventory_object= ecs::spawn(world(), inventory_menu_component());
		hotbar=ecs::spawn(world(),items::container_recipe(v2::Coord2(0,0),v2::Coord2(6,1)));
		givestartitems("plank");
	}
	void update() {


		if (CtxName::ctx.Inp->getKey('e').pressed) {
			world().write_command(ui::open_menu(inventory_object.get_component<ui::menu_component>()));
		}
		if (CtxName::ctx.Inp->getKey('1').pressed) {

			selected_ind =items::container_index(0,0);
		}
		if (CtxName::ctx.Inp->getKey('2').pressed) {

			selected_ind = items::container_index(1, 0);
		}
		if (CtxName::ctx.Inp->getKey('3').pressed) {

			selected_ind = items::container_index(2, 0);
		}if (CtxName::ctx.Inp->getKey('4').pressed) {

			selected_ind = items::container_index(3, 0);
		}
		if (CtxName::ctx.Inp->getKey('5').pressed) {

			selected_ind = items::container_index(4, 0);
		}
		if (CtxName::ctx.Inp->getKey('6').pressed) {
			selected_ind = items::container_index(5, 0);

		}
	}
};



// !playerinventory_H
