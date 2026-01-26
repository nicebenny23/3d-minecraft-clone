#include "recipe_transactions.h"
#include "FakeItem.h"
#pragma once
namespace items {
	struct crafting_slot_tag :ecs::component {


		ecs::obj crafter;
		crafting_slot_tag(ecs::obj craft) :crafter(craft) {

		}
	};

	struct crafter:ecs::component {
		crafter(RecipeBinder binder) :binder(binder), wanted(stn::None) {

		}
		
		void start() {
			binder.output.ensure_component<crafting_slot_tag>(owner());
		}
		RecipeBinder binder;
		stn::Option<ItemRecipe> wanted;
		void set_state() {
			stn::Option<ItemRecipe> recipe = best_booklet_recipe(binder.list,binder.input.get_component<container>(), stn::None);
				binder.output.get_component<display_item>().display = recipe.member(&ItemRecipe::output).copied();
			
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