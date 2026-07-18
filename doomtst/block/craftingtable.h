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
#include "chest.h"
#pragma once 

namespace items {

	struct CraftingTableItem : item_type {
		std::string name() const {
			return ("crafting_table");
		}
		item_traits traits(const ecs::Ecs& world) const {
				return item_traits::block_item(
				renderer::TexturePath("images\\craftingtabletop.png"),
				world.get_resource<BlockRegistry>().get_id("crafting_table")
			);
		}
	};
	struct crafting_table_loot_table :items::LootTable {
		items::LootDrops drops_for(items::ItemTypes& types,ecs::obj dropping) const {
			return items::LootDrops({ items::loot_element(types.insert<CraftingTableItem>(),1,types) });
		}
	};
}
namespace blocks {

	struct CraftingTableMenuRecipe {
		ecs::Constrained<items::Container> input_slots;

		
		void apply(ecs::obj& ent) const {
			ecs::obj menu_object= ecs::spawn_emplaced<ui::MenuRecipe>(ent.world());
			menu_object.apply_recipe(player::inventory_slots_pannel_recipe());
			ecs::obj bg = menu_object.spawn_child_emplaced<ui::ImageSpawner>(renderer::TexturePath("images\\menutex.png"),
			geo::Box2d(v2::Vec2(.27f, .15f)/2, v2::Vec2(.35f, .29f))
			,0);
			ent.add_component<player::OpenMenuOnClick>(ecs::ConstrainedHandle<ui::MenuComponent>(menu_object));
			menu_object.spawn_child_emplaced<items::ContainerDisplayRecipe>(v2::Coord2(4, 3), input_slots);
			ecs::Constrained<items::crafter> crafter = ent
				.spawn_child_emplaced< items::CrafterRecipe>(input_slots.object(), stn::array({std::filesystem::path("crafting\\2x2craft.txt"), std::filesystem::path("crafting\\3x3craft.txt")}));
			menu_object.spawn_child_emplaced<items::CraftingSlotDisplaySpawner>(v2::Coord2(8, 4), crafter);
		}
	};

	struct CraftingTableBlock:BlockType {
		void apply(ecs::obj& block) const override {
			ecs::Constrained<items::Container> input_slots(ecs::spawn(block.world(), items::container_recipe(ui::TableBounds(3,3))));
			block.add_component<stored_container>(input_slots);
			block.apply_recipe(CraftingTableMenuRecipe{ .input_slots = input_slots});
			block.apply_recipe<>(items::loot_table_recipe<items::crafting_table_loot_table>);
		}
		std::string name() const {
			return std::string("crafting_table");
		}
		BlockMeshTraits traits(BlockTextureRegistry& registry) const{
			block_texture crafting_table_top = registry.get_texture("images\\craftingtabletop.png"); 
			block_texture crafting_table_bottom = registry.get_texture("images\\craftingtable.png");
			block_texture crafting_table_side = registry.get_texture("images\\craftingtableside.png");	
			return BlockMeshTraits(v3::unit_scale, false, crafting_table_side, crafting_table_side, crafting_table_top, crafting_table_bottom, crafting_table_side, crafting_table_side);
		}
		stn::Option<SolidBlockTraits> solid_traits_for() const override {
			return SolidBlockTraits(3, 0);
		}
		void read_from_bytes(ecs::obj block, stn::file_handle& handle)const  override {
			items::container_id id = stn::file_serializer<items::container_id>().read(handle);
			ecs::Constrained<items::Container> container_slot(block.world().get_resource<items::WorldContainers>()[id]);
			block.add_component<stored_container>(container_slot);
			block.apply_recipe(CraftingTableMenuRecipe{ .input_slots= container_slot });
			block.apply_recipe(items::loot_table_recipe<items::crafting_table_loot_table>);
		}
		void write_to_bytes(ecs::obj block, stn::file_handle& handle) const override {
			items::container_id id = block.get_component<stored_container>().stored.get<items::Container>().id;
			stn::file_serializer<items::container_id>().write(id, handle);

		}
	};


}