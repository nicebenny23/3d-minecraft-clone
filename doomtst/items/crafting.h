#include "recipe.h"
#include "container_transactions.h"
#include "menu.h"
#include "../math/vector2.h"
#include "recipe_file.h"
#include "recipe_transactions.h"
#include "FakeItem.h"
#include "cursor_slot.h"
#include "menu.h"
#include "Container.h"
#include "ItemSlot.h"
#include "ItemUi.h"
#include "../renderer/ui.h"
#include "../util/Option.h"
#include "../game/ecs/filtered_object.h"

#pragma once

namespace items {

	struct Crafter :ecs::component {
		Crafter(RecipeBinder binder) :binder(binder), wanted(stn::None) {

		}

		
		RecipeBinder binder;
		stn::Option<ItemRecipe> wanted;
		void set_state() {
			wanted = best_booklet_recipe(binder.list, binder.input, stn::None);

		}
	};
	struct CraftingSlotDisplayer :ecs::component {
		ecs::Constrained<Crafter> crafter_comp;
		ecs::Constrained<ui::InteractionState, ItemIcon, ItemCountDisplay> display;
		CraftingSlotDisplayer(ecs::Constrained <Crafter > craft, ecs::Constrained <ui::InteractionState, ItemIcon, ItemCountDisplay> display_item) :crafter_comp(craft), display(display_item) {

		}
		
	};
	struct CraftingSlotDisplaySpawner {
		v2::Coord2 pos;
		ecs::Constrained<Crafter> crafter;
		void apply(ecs::obj& entity) const {
			entity.apply_recipe(ui::UiSpawner(geo::unit_box_2d, 1));
			ecs::Constrained<ui::InteractionState, ItemIcon, ItemCountDisplay> display = entity.spawn_child_emplaced<FakeItemSlotDispaySpawner>(pos);
			entity.add_component <CraftingSlotDisplayer>(crafter, display);
		}

	};

	struct RunCrafts :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View< Crafter> craft_view(world);
			for (auto&& [crafter_component] : craft_view) {
				crafter_component.set_state();
			}
			if (world.get_resource<ui::MenuState>().no_menu_open()) {
				return;
			}
			ecs::Constrained<ElementSlot> cursor_obj = world.get_resource<items::cursor_container>().primary_slot();
			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			//		cursor_obj.get_component<ui::UiBounds>().local.center = world.ensure_resource<userinput::InputManager>().mouse_position;
			for (auto&& [crafting_slot_display] : ecs::View< items::CraftingSlotDisplayer>(world)) {
				stn::Option<item_entry> entry = crafting_slot_display.crafter_comp.get_component<Crafter>().wanted.member(&ItemRecipe::output);
				crafting_slot_display.display.get<ItemCountDisplay>().count = entry.member(&item_entry::count);
				crafting_slot_display.display.get<ItemIcon>().displayed_id = entry.member(&item_entry::id);
			}
			for (auto&& [crafting_slot_display] : ecs::View< items::CraftingSlotDisplayer>(world)) {

				if (crafting_slot_display.display.get_component<ui::InteractionState>().left_clicked) {
					Crafter& crafter_comp = crafting_slot_display.crafter_comp.get_component<Crafter>();
					auto auto_val = build_recipe_from_booklet(crafter_comp.binder.list, crafter_comp.binder.input, cursor_obj);
					if (auto_val) {
						auto_val.unwrap().apply(world);
					}
				}
			}
		}
	};
	struct CrafterRecipe {

		CrafterRecipe(ecs::Constrained<items::Container> input, stn::array<std::filesystem::path> crafting_path) :input_container(input), paths(crafting_path) {

		}
		void apply(ecs::obj& entity) const {
			ItemRecipes recipes;
			recipes.size = input_container.get_component<Container>().size;
			for (const std::filesystem::path& path : paths) {
				json::Value booklet = json::parse_for_file(path);
				recipes.recipe_list.append(recipe_booklet_from_path(recipes.size, booklet, entity.world()).recipe_list);
			}
			entity.add_component<Crafter>(RecipeBinder(input_container, recipes));
		}
		stn::array<std::filesystem::path> paths;
		ecs::Constrained<items::Container> input_container;
	};
};