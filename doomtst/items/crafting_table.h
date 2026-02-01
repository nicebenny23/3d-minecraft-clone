#include "recipe_transactions.h"
#include "FakeItem.h"
#include "cursor_slot.h"
#pragma once
namespace items {
	struct crafting_slot_tag :ecs::component {


		ecs::obj crafter;
		crafting_slot_tag(ecs::obj craft) :crafter(craft) {

		}
	};

	struct crafter :ecs::component {
		crafter(RecipeBinder binder) :binder(binder), wanted(stn::None) {

		}

		void start() {
			binder.output.ensure_component<crafting_slot_tag>(owner());
		}
		RecipeBinder binder;
		stn::Option<ItemRecipe> wanted;
		void set_state() {
			stn::Option<ItemRecipe> recipe = best_booklet_recipe(binder.list, binder.input.get_component<container>(), stn::None);
			binder.output.get_component<display_item>().display = recipe.member(&ItemRecipe::output).copied();

		}
	};
	struct cursor_crafter :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::obj cursor_obj = world.get_resource<items::cursor_container>().unwrap().primary_slot();
			ElementSlot& cursor_slot = cursor_obj.get_component<ElementSlot>();
			cursor_obj.get_component<ui::UiBounds>().local.center = world.ensure_resource<userinput::InputManager>().mouse_position;
			for (auto&& [item_decal, interaction_state, crafting_slot_tag] : ecs::View<ItemSlotDecal, ui::InteractionState, items::crafting_slot_tag>(world)) {
				if (interaction_state.left_clicked) {
					crafter& crafter_comp = crafting_slot_tag.crafter.get_component<crafter>();
					auto auto_val = build_recipe_from_booklet(crafter_comp.binder.list, crafter_comp.binder.input.get_component<container>(), cursor_slot);

					if (auto_val) {
						auto_val.unwrap().apply(world);
					}
				}
			}
		}
	};

	struct run_crafts :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<crafter> craft_view(world);
			for (auto&&[crafter_component]:craft_view ) {
				crafter_component.set_state();
			}
		}
	};









}