#include "recipe.h"
#include "container_transactions.h"
#include "menu.h"
#include "../math/vector2.h"
#include "recipe_file.h"
#include "crafting_table.h"
#pragma once

namespace items {
	
	struct CrafterRecipe: ecs::Recipe {

		CrafterRecipe(ecs::obj input, ecs::obj output, std::filesystem::path crafting_path) :input_container(input), output_slot(output), path(crafting_path){

		}
		void apply(ecs::obj& entity) {
			json::Value booklet= json::parse_for_file(path);
			ItemRecipes recipes = recipe_booklet_from_path(input_container.get_component<container>().size, booklet, entity.world());
			entity.add_component<crafter>(RecipeBinder(input_container, output_slot, recipes));
		}
		std::filesystem::path path;
		ecs::obj input_container;
		ecs::obj output_slot;
	};
};