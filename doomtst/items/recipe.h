#include "Item.h"
#include "Container.h"
#pragma once
namespace items {

	struct ItemRecipe {
		v2::Coord2 size;
		stn::array<stn::Option<item_entry>> item_list;
		item_entry output;
		stn::Option<item_entry> from_cell(container_index cell) {
			if (cell.fits_in(size)) {
				return item_list[cell.index_in(size)];
			}
			stn::throw_logic_error("cell {} out of range in {}", cell.coord , size);
		}
		ItemRecipe(v2::Coord2 grid_size, stn::array<stn::Option<item_entry>> input_entries, item_entry output_entry) :
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


	inline stn::array<ItemRecipe> spread_out(ItemRecipe recipe,v2::Coord2 spread_size) {

		if (spread_size.x < recipe.size.x || spread_size.y < recipe.size.y) {
			stn::throw_logic_error("cannot spread out to {} from {}", recipe.size, spread_size);
		}
		v2::Coord2 positions = spread_size - recipe.size;
		stn::array<ItemRecipe> recipes;
		for (int x = 0; x <= positions.x; x++) {
			for (int y = 0; y <= positions.y; y++) {
				v2::Coord2 shift_offset = v2::Coord2(x, y);
				stn::array < stn::Option<item_entry>> entries(spread_size.x*spread_size.y);
				for (int container_y = 0; container_y < recipe.size.y; container_y++) {
					for (int container_x = 0; container_x < recipe.size.x; container_x++) {
						
						v2::Coord2 real_location(container_x, container_y);
						container_index shifted(real_location + shift_offset);
						size_t real_index=shifted.index_in(spread_size);
						//all non_initilized ones will be set to None
						entries.reach(real_index) = recipe.from_cell(container_index(real_location));
					}
				}
				recipes.push(ItemRecipe(spread_size, std::move(entries), recipe.output));
			}
		}
		return recipes;
	}



	struct ItemRecipes {
		v2::Coord2 size;
		stn::array<ItemRecipe> recipe_list;
		ItemRecipes() {

		}

		ItemRecipes(stn::array<ItemRecipe>&& recipes) :recipe_list(std::move(recipes)), size(recipes.first().size) {

		}
		ItemRecipes(const stn::array<ItemRecipe>& recipes) :recipe_list(recipes), size(recipes.first().size) {

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
		ecs::obj output;
		ecs::obj input;
		ItemRecipes list;
		RecipeBinder(ecs::obj in, ecs::obj out, ItemRecipes list) :input(in), output(out), list(list) {

		}
	};
}