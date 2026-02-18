#include "recipe.h"
#include "container_transactions.h"
#include "menu.h"
#include "../math/vector2.h"
#include "recipe_file.h"
#include "crafting_table.h"
#pragma once

namespace items {
	
	struct CrafterRecipe{

		CrafterRecipe(ecs::obj input, stn::array<std::filesystem::path> crafting_path) :input_container(input), paths(crafting_path){

		}
		void apply(ecs::obj& entity) {
			ItemRecipes recipes;
			recipes.size = input_container.get_component<container>().size;
			for (std::filesystem::path& path: paths) {
				json::Value booklet = json::parse_for_file(path);
				recipes.recipe_list.append(recipe_booklet_from_path(recipes.size, booklet, entity.world()).recipe_list);
			}
			entity.add_component<crafter>(RecipeBinder(input_container, recipes));
		}
		stn::array<std::filesystem::path> paths;
		ecs::obj input_container;
	};
};