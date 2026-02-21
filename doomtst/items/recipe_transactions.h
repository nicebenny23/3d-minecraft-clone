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
			AddToSlotPlan(output, recipe.output).apply(world);
			container& cont = input.get_component<container>();
			for (size_t i = 0; i < cont.slots.length(); i++) {
				if (recipe.item_list[i]) {
					TakeFromSlot(recipe.item_list[i].unwrap(), cont.slots[i].get()).apply(world);
				}
			}
		}
		size_t nonempty_slot_count() const {
			return recipe.nonempty_slots();
		}
	};



	inline	bool can_build(const ItemRecipe& item_recipe, const container& input_container, stn::Option<items::item_entry> output_slot) {
		if (input_container.size != item_recipe.size) {
			stn::throw_logic_error("recipe size {} must match container size {}", item_recipe.size, input_container.size);
		}
		for (size_t index = 0; index < input_container.slots.length(); index++) {
			const ElementSlot& element = input_container.slots[index].get_component<ElementSlot>();
			bool should_be_empty = item_recipe.item_list[index].is_none();
			if (should_be_empty != element.empty()) {
				return false;
			}
			if (!should_be_empty) {
				item_entry item = item_recipe.item_list[index].unwrap();
				if (element.empty()) {
					return false;
				}
				const item_stack& stck = element.element().unwrap().get_component<item_stack>();
				if (!stck.can_fit(item)) {
					return false;
				}
			}
		}
		return true;
	}

	inline stn::Option<ItemRecipe> best_booklet_recipe(ItemRecipes recipes, const container& input, stn::Option<item_entry> output) {

		for (const ItemRecipe& recipe : recipes) {
			if (can_build(recipe, input, output)) {
				return recipe;
			}
		}
		return stn::None;
	}

	inline stn::Option<craft_recipe> build_recipe_from_booklet(ItemRecipes recipes, container& input, ElementSlot& output) {

		stn::Option<ItemRecipe> recipe = best_booklet_recipe(recipes, input, output.entry());
		if (!recipe) {
			return stn::None;
		}
		return craft_recipe(recipe.unwrap(), input, output);
	}

}