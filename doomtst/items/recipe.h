#include "Item.h"
#include "Container.h"
#pragma once
namespace items {

	struct ItemRecipe {
		v2::Coord2 size;
		stn::array<stn::Option<item_entry>> item_list;
		item_entry output;

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
		size_t nonempty_slot_count() const {
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
		ecs::obj container_output;
		ecs::obj container_input;
		ItemRecipes list;
	};
}