#include "recipe.h"
#include "slot_transactions.h"
#pragma once
namespace items {
	struct craft_recipe {

		craft_recipe(const ItemRecipe& item_recipe, ecs::Constrained <Container> input_container, ecs::Constrained <ElementSlot> output_slot) :input(input_container), output(output_slot), recipe(item_recipe) {

		}
		ItemRecipe recipe;
		ecs::Constrained<Container> input;
		ecs::Constrained<ElementSlot> output;

		void apply(ecs::Ecs& world) {
			AddToSlotPlan::build(AddToSlotRequest{ .entry = recipe.output,.slot = output}).unwrap().apply();
			Container& cont = input.get_component<Container>();
			for (size_t i = 0; i < cont.slots.length(); i++) {
				if (recipe.item_list[i]) {
					RemoveFromSlotRequest{.slot= cont.slots[i].object(),.count = recipe.item_list[i].unwrap().count}.build().unwrap().apply();
				}
			}
		}
		size_t nonempty_slot_count() const {
			return recipe.nonempty_slots();
		}
	};



	inline	bool can_build(const ItemRecipe& item_recipe, ecs::Constrained <Container> input_container, stn::Option<items::item_entry> output_slot) {
		Container& input = input_container.get<Container>();
		if (input.size != item_recipe.size) {
			stn::throw_logic_error("recipe size {} must match container size {}", item_recipe.size, input.size);
		}
		for (size_t index = 0; index < input.slots.length(); index++) {
			const ElementSlot& element = input.slots[index].get_component<ElementSlot>();
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
				if (!stck.contained_entry().can_produce(item)) {
					return false;
				}
			}
		}
		if (output_slot) {
			
			return output_slot.unwrap().can_accept(item_recipe.output, input.types());
		}
		return true;
	}

	inline stn::Option<ItemRecipe> best_booklet_recipe(ItemRecipes recipes, const ecs::Constrained <Container>& input, stn::Option<item_entry> output) {
		const ItemTypes& types = input.world().get_resource<ItemTypes>();
		for (const ItemRecipe& recipe : recipes) {
			if (can_build(recipe, input, output)) {
				return recipe;
			}
		}
		return stn::None;
	}

	inline stn::Option<craft_recipe> build_recipe_from_booklet(ItemRecipes recipes, ecs::Constrained < Container> input, ecs::Constrained <ElementSlot> output) {

		stn::Option<ItemRecipe> recipe = best_booklet_recipe(recipes, input, output.get<ElementSlot>().entry());
		if (!recipe) {
			return stn::None;
		}
		return craft_recipe(recipe.unwrap(), input, output);
	}

}