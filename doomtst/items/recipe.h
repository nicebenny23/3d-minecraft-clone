#include "Item.h"
#include "Container.h"
#pragma once
namespace items {

	struct ItemRecipe {
		v2::Coord2 size;
		stn::array<stn::Option<item_entry>> item_list;
		item_entry output;
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
		const_iterator begin() const{
			return item_list.begin();
		}
		const_iterator end() const {
			return item_list.end();
		}
		size_t nonempty_slots() const {
			size_t count=0;
			for (const stn::Option<item_entry>& slot: item_list) {
				if (slot.is_some()) {
					count++;
				}
			}
			return count;
		}
	};  
	
	





	struct ItemRecipes{ 
		v2::Coord2 size;
		stn::array<ItemRecipe> recipe_list;
		ItemRecipes(){
	
		}
		ItemRecipes(const stn::array<ItemRecipe>& recipes):recipe_list(recipes),size(recipes.first().size) {

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