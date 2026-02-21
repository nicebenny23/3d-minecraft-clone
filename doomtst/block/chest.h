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

	struct chest_item : item_type {
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits(
				std::string_view("chest"),
				renderer::TexturePath("images\\chest.png", "chest_image"),
				world.get_resource<BlockRegistry>().get_id("chest")
			);
		}
	};
}
namespace blocks {

	struct ChestMenuRecipe {
		ecs::Constrained<items::container> chest_slots;


		void apply(ecs::obj& ent) const {
			ecs::obj menu_object = ecs::spawn_emplaced<ui::MenuRecipe>(ent.world());
			menu_object.apply_recipe(player::inventory_slots_pannel_recipe()); ecs::obj bg = menu_object.spawn_child<ui::ui_image_spawner>(renderer::TexturePath("images\\menutex.png", "menu_texture"), geo::Box2d(v2::Vec2(.2f, .2f), v2::Vec2(.5f, .35f)), -1);
			ent.add_component<player::OpenMenuOnClick>(ecs::ConstrainedHandle<ui::menu_component>(menu_object));
			menu_object.spawn_child<items::ContainerDisplayRecipe>(v2::Coord2(-4, 3), chest_slots);

			menu_object.get_component<ui::UiEnabled>().disable();
		}
	};
	struct stored_container:ecs::component {
		ecs::Constrained<items::container> stored;
		stored_container(ecs::Constrained<items::container> stored_item) :stored(stored_item) {

		}
	};
	struct ChestBlock :BlockType {
		BlockTraits traits() const{
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale / 2, false, blocks::chestfront, blocks::chestside, blocks::chestside, blocks::chestside, blocks::chestside, blocks::chestfront)
			);
		}
		void apply(ecs::obj& block) const override {
			ecs::Constrained<items::container> chest_slots(ecs::spawn(block.world(), items::container_recipe(v2::Coord2(8, 4))));
			block.add_component<stored_container>(chest_slots);
			ecs::Constrained<items::container> player_main_slots(player::player_for(block.world()).get_component<player::inventory>().slots);
			block.apply_recipe(ChestMenuRecipe{ .chest_slots = chest_slots });
			block.add_component<items::loot_table>().add("chest", 1);
		}
		void read_from_bytes(ecs::obj block,stn::file_handle& handle)const  override {
			ecs::Constrained<items::container> player_main_slots(player::player_for(block.world()).get_component<player::inventory>().slots);
			ecs::Constrained<items::container> container_slot(block.world().get_resource<items::WorldContainers>()[stn::file_serializer<items::container_id>().read(handle)]);
			block.add_component<stored_container>(container_slot);
			block.apply_recipe(ChestMenuRecipe{.chest_slots = container_slot });
			block.add_component<items::loot_table>().add("chest", 1);
		}
		void write_to_bytes(ecs::obj block,stn::file_handle& handle) const {
			stn::file_serializer<items::container_id>().write(block.get_component<stored_container>().stored.get<items::container>().id,handle);
		}
		std::string name() const {
			return std::string("chest");
		}
	};



}