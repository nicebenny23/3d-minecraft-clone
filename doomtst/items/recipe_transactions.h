#include "recipe.h"
#include "slot_transactions.h"
#pragma once
namespace items {
	struct craft_recipe {

		craft_recipe(const ItemRecipe& item_recipe, container& input_container, ElementSlot& output_slot) :input(input_container.owner()), output(output_slot.owner()), recipe(item_recipe) {

		}
		ItemRecipe recipe;
		ecs::obj input;
		ecs::obj output;
		void apply(ecs::Ecs& world) {
			AddToSlot(recipe.output,output).apply(world);
			container& cont = input.get_component<container>();
			for ( size_t i= 0; i < cont.slots.length(); i++) {
				if (recipe.item_list[i]) {
					TakeFromSlot(recipe.item_list[i].unwrap(), cont.slots[i].get()).apply(world);
				}
			}
		}
		size_t nonempty_slot_count() const{
			return recipe.nonempty_slot_count();
		}
	};



	struct build_recipe {
		ItemRecipe recipe;
		ecs::obj input;
		ecs::obj output;

		build_recipe(const ItemRecipe& item_recipe, container& input_container, ElementSlot& output_slot) :input(input_container.owner()), output(output_slot.owner()), recipe(item_recipe) {

		}
		stn::Option<craft_recipe> plan() {
			container& cont = input.get_component<container>();
			if (cont.size != recipe.size) {
				stn::throw_logic_error("recipe size {} must match container size {}", recipe.size, cont.size);
			}
			for (size_t index = 0; index < cont.slots.length(); index++) {
				if (recipe.item_list[index]) {
					item_entry item = recipe.item_list[index].unwrap();
					ElementSlot& element = cont.slots[index].get_component<ElementSlot>();
					if (element.empty()) {
						return stn::None;
					}
					item_stack& stck = element.element().unwrap().get_component<item_stack>();
					if (!stck.can_fit(item)) {
						return stn::None;
					}

				}
			}
			return craft_recipe(recipe,input.get_component<container>(),output.get_component<ElementSlot>());
		}
	};
	struct build_recipe_from_booklet {
		ItemRecipes recipes;
		ecs::obj input;
		ecs::obj output;
		build_recipe_from_booklet(const ItemRecipes& item_recipes, container& input_container, ElementSlot& output_slot) :input(input_container.owner()), output(output_slot.owner()), recipes(item_recipes) {

		}
		stn::Option<craft_recipe> plan() {
			stn::Option < craft_recipe> maximal_recipe;
			for (const ItemRecipe& recipe : recipes) {
				stn::Option<craft_recipe> craft_plan = build_recipe(recipe, input.get_component<container>(), output.get_component<ElementSlot>()).plan();
				if (maximal_recipe.map(&craft_recipe::nonempty_slot_count) < craft_plan.map(&craft_recipe::nonempty_slot_count)) {
					maximal_recipe = craft_plan;
				}
			}
			return maximal_recipe;
		}
	};

}