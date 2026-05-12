#include "Item.h"
#include "Container.h"
#include "../renderer/ui_table.h"
#pragma once
namespace items {

	struct ItemRecipe {
		ui::TableBounds size;
		stn::array<stn::Option<item_entry>> item_list;
		item_entry output;
		stn::Option<item_entry> from_cell(v2::UVec2 cell) {
			return item_list[size.index(cell)];
		}
		ItemRecipe(ui::TableBounds grid_size, stn::array<stn::Option<item_entry>> input_entries, item_entry output_entry) :
			size(grid_size), item_list(input_entries), output(output_entry) {

		}
		using iterator = decltype(item_list)::iterator;
		using const_iterator = decltype(item_list)::const_iterator;
		iterator begin() {
			return item_list.begin();
		}
		iterator end() {
			return item_list.end();
		}
		const_iterator begin() const {
			return item_list.begin();
		}
		const_iterator end() const {
			return item_list.end();
		}
		size_t nonempty_slots() const {
			size_t count = 0;
			for (const stn::Option<item_entry>& slot : item_list) {
				if (slot.is_some()) {
					count++;
				}
			}
			return count;
		}
	};


	inline stn::array<ItemRecipe> spread_out(ItemRecipe recipe, ui::TableBounds spread_size) {

		if (recipe.size.contains(spread_size)) {
			stn::throw_logic_error("cannot spread out to {} from {}", recipe.size, spread_size);
		}
		ui::TableBounds positions((spread_size.table - recipe.size.table).unwrap() + v2::UVec2(1, 1));
		stn::array<ItemRecipe> recipes;
		for (v2::UVec2 shift_offset:positions) {
			stn::array < stn::Option<item_entry>> entries(spread_size.entries());
			for (v2::UVec2 real_location : recipe.size) {
				v2::UVec2 shifted(real_location + shift_offset);
				size_t real_index = spread_size.index(shifted);
				//all non_initilized ones will be set to None
				entries[real_index] = recipe.from_cell(real_location);
			}
			recipes.push(ItemRecipe(spread_size, std::move(entries), recipe.output));

		}
		return recipes;
	}



struct ItemRecipes {
	ui::TableBounds size;
	stn::array<ItemRecipe> recipe_list;
	ItemRecipes(stn::array<ItemRecipe>&& recipes) :recipe_list(std::move(recipes)), size(recipes.first().size) {

	}
	ItemRecipes(const stn::array<ItemRecipe>& recipes) :recipe_list(recipes), size(recipes.first().size) {

	}
	ItemRecipes() {

	}
	using iterator = decltype(recipe_list)::iterator;
	using const_iterator = decltype(recipe_list)::const_iterator;
	iterator begin() {
		return recipe_list.begin();
	}
	iterator end() {
		return recipe_list.end();
	}
	const_iterator begin() const {
		return recipe_list.begin();
	}
	const_iterator end() const {
		return recipe_list.end();
	}
};

struct RecipeBinder {
	ecs::obj input;
	ItemRecipes list;
	RecipeBinder(ecs::obj in, ItemRecipes list) :input(in), list(list) {

	}
};
}