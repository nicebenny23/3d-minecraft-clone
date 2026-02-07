#include "recipe.h"
#include "container_transactions.h"
#include "menu.h"
#include "../math/vector2.h"
#include "recipe_file.h"
#include "crafting_table.h"
#pragma once

namespace items {
	struct crafting_executor{
		RecipeBinder recipes;
		item_entry current_craft() {

		}

	};
	struct make_crafting_table: ecs::Recipe {
		make_crafting_table(ecs::obj input, ecs::obj output) :input_container(input), output_slot(output) {

		}
		void apply(ecs::obj& entity) {
			json::Value booklet= json::parse_for_file(std::filesystem::path("crafting\\2x2craft.txt"));
			ItemRecipes recipes = recipe_booklet_from_path(booklet,entity.world());
			entity.add_component<crafter>(RecipeBinder(input_container, output_slot, recipes));
		}

		ecs::obj input_container;
		ecs::obj output_slot;
	};
};