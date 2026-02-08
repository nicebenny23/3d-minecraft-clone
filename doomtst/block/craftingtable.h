#include "block.h"
#include "../items/recipe.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#include "block.h"
#include "block_registry.h"
#include "../items/loottable.h"
#include "../items/recipe_file.h"
#include "../items/recipe_transactions.h"
#pragma once 
namespace blocks {

	struct CraftingTableRecipe :ecs::Recipe {

	};
	struct CraftingTableBlock:BlockType {
		void apply(ecs::obj& block) override {
			block.add_component<items::loot_table>().add("moss", 1);
		}
	};
	template<>
	inline constexpr BlockTraits BlockInfo<CraftingTableBlock> = BlockTraits(
		BlockMeshTraits(v3::unit_scale, false, craftingtableside, craftingtableside, mosstex, mosstex, craftingtableside, craftingtableside)
	);

}