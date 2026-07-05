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

	struct ChestItem : item_type {
		std::string name() const {
			return "chest";
		}
		item_traits traits(const ecs::Ecs& world) const {
			return item_traits::block_item(
				renderer::TexturePath("images\\chest.png"),
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
			menu_object.apply_recipe(player::inventory_slots_pannel_recipe()); ecs::obj bg = menu_object.spawn_child_emplaced<ui::ImageSpawner>(renderer::TexturePath("images\\menutex.png"), geo::Box2d(v2::Vec2(.2f, .1f)/2, v2::Vec2(.26f, .2f)), 0);
			ent.add_component<player::OpenMenuOnClick>(ecs::ConstrainedHandle<ui::MenuComponent>(menu_object));
			menu_object.spawn_child_emplaced<items::ContainerDisplayRecipe>(v2::Coord2(0,2), chest_slots);
		}
	};
	struct stored_container:ecs::component {
		ecs::Constrained<items::container> stored;
		stored_container(ecs::Constrained<items::container> stored_item) :stored(stored_item) {

		}
	};
	struct chest_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types) {
			return items::LootDrops({ items::loot_element(types.insert<items::ChestItem>(),1,types)});
		}
	};
	struct ChestBlock :BlockType {
		BlockTraits traits(BlockTextureRegistry& registry) const{
			block_texture chest_side = registry.get_texture("images\\chest_sides.png");
			block_texture chest_top = registry.get_texture("images\\chest_top.png");
			block_texture chest_front = registry.get_texture("images\\chest_front.png");
			return BlockTraits(
				BlockMeshTraits(v3::unit_scale, false, chest_side, chest_side, chest_top, chest_top, chest_front, chest_side)
			);
		}
		void apply(ecs::obj& block) const override {
			ecs::Constrained<items::container> chest_slots(ecs::spawn(block.world(), items::container_recipe(ui::TableBounds(6,2))));
			block.add_component<stored_container>(chest_slots);
			ecs::Constrained<items::container> player_main_slots(player::player_for(block.world()).get_component<player::inventory>().slots);
			block.apply_recipe(ChestMenuRecipe{ .chest_slots = chest_slots });
			block.apply_recipe(items::loot_table_recipe<chest_loot_table>);
		}
		void read_from_bytes(ecs::obj block,stn::file_handle& handle)const  override {
			items::container_id id = stn::file_serializer<items::container_id>().read(handle);
			ecs::Constrained<items::container> player_main_slots(player::player_for(block.world()).get_component<player::inventory>().slots);
			ecs::Constrained<items::container> container_slot(block.world().get_resource<items::WorldContainers>()[id]);
			block.add_component<stored_container>(container_slot);
			block.apply_recipe(ChestMenuRecipe{.chest_slots = container_slot });
			block.apply_recipe(items::loot_table_recipe<chest_loot_table>);
		}
		void write_to_bytes(ecs::obj block,stn::file_handle& handle) const override {
			items::container_id id = block.get_component<stored_container>().stored.get<items::container>().id;
			stn::file_serializer<items::container_id>().write(id,handle);
			int l = 3;
		}
		std::string name() const {
			return std::string("chest");
		}
	};



}