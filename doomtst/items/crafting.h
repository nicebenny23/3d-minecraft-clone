#include "recipe.h"
#include "container_transactions.h"
#include "menu.h"
#include "../math/vector2.h"
#pragma once

namespace items {
	struct crafting_executor{
		RecipeBinder recipes;
		item_entry current_craft() {

		}

	};
	struct craftingtablecomp : ecs::component {

	};
};