#include "recipe.h"
#include "../json/json.h"
#pragma once
namespace items {
	inline v2::Coord2 ParseVector2(const json::Value& value) {
		return v2::Coord2(value.get_subobject_as<int64_t>("x").unwrap(), value.get_subobject_as<int64_t>("y").unwrap());
	}
	inline v2::UVec2 ParseUVector2(const json::Value& value) {
		return v2::UVec2(value.get_subobject_as<int64_t>("x").unwrap(), value.get_subobject_as<int64_t>("y").unwrap());
	}
	struct ParseItemEntry {
		ecs::Ecs& world;
		ParseItemEntry(ecs::Ecs& ecs) :world(ecs) {
		}
		item_entry operator()(const json::Value& item) {
			ItemTypes& item_register = world.insert_resource<ItemTypes>();
			std::string item_name = item.get_subobject_as<std::string>("id").expect("item block_id should exist");
			size_t count = item.get_subobject_as<json::Integer>("count").copied().unwrap_or(1);
			return item_entry(item_register.from_name(item_name), count, item_register);
		}
	};
	struct ItemRecipeFromJson {
		stn::array<ItemRecipe> operator()(const json::Value& value) {
			const json::Object& obj = value.extract<json::Object>().expect("json must be an object");
			v2::UVec2 coord = ParseUVector2(obj.get("size").expect("size must exist in a recipe"));
			const json::Array& arr = value.get_subobject_as<json::Array>("input").expect("recipe must have an input");
			if (arr.length() != coord.x * coord.y) {
				stn::throw_logic_error("json element list may not have {} while it has a recipe of size{}", arr.length(), coord.x * coord.y);
			}
			stn::array<stn::Option<item_entry>> crafting_list;
			ParseItemEntry item_parser(world);
			for (const stn::box<json::Value>& item : arr) {
				crafting_list.push(json::ParseOption(item_parser)(*item));
			}
			item_entry output = item_parser(obj.get("output").expect("recipe must have an output"));
			return spread_out(ItemRecipe(coord, std::move(crafting_list), output),size);
		}
		ecs::Ecs& world;
		ItemRecipeFromJson(ecs::Ecs& ecs, ui::TableBounds expand_to) :world(ecs),size(expand_to){
		}
		ui::TableBounds size;
	};
	inline ItemRecipes recipe_booklet_from_path(ui::TableBounds size,const json::Value& value, ecs::Ecs& world) {
		return ItemRecipes(json::ComposeArray<ItemRecipeFromJson>(ItemRecipeFromJson(world,size))(value));
	}
}
