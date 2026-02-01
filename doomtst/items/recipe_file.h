#include "recipe.h"
#include "../json/json.h"
#pragma once
namespace items {

	inline v2::Coord2 JsonParseVector2(const json::Value& value) {
		return v2::Coord2(value.get_subobject_as<int64_t>("x").unwrap(), value.get_subobject_as<int64_t>("y").unwrap());
	}

	inline 	item_entry item_entry_from_json(const json::Value& item,ecs::Ecs& world) {

		item_types& item_register = world.ensure_resource<item_types>();
			std::string item_name = item.get_subobject_as<std::string>("id").expect("item block_id should exist");
			size_t count = item.get_subobject("count")
		.expect("item count should exist")
		.as_unsigned()
		.expect("item count should not be negitive");
		return item_entry(item_register.from_name(item_name), count,world.ensure_resource<item_types>());
	}

	inline stn::Option<item_entry> mabye_item_entry_from_json(const json::Value& item, ecs::Ecs& world) {
		if (item.is<stn::NoneType>()) {
			return stn::None;
		}
		return item_entry_from_json(item,world);
	}
	inline ItemRecipe	recipe_from_json(const json::Value& value, ecs::Ecs& world) {
		const json::Object& obj= value.extract<json::Object>().expect("json must be an object");
		v2::Coord2 coord = JsonParseVector2(obj.get("size").expect("size must exist in a recipe"));
		const json::Array& arr = value.get_subobject_as<json::Array>("input").expect("recipe must have an input");
		if (arr.length()!=coord.x*coord.y) {
			stn::throw_logic_error("json element list may not have {} while it has a recipe of size{}", arr.length(), coord.x * coord.y);
		}
		stn::array<stn::Option<item_entry>> crafting_list;
		for (const stn::box<json::Value>& item:arr) {
			crafting_list.push(mabye_item_entry_from_json(*item,world));
		}
		item_entry output = item_entry_from_json(obj.get("output").expect("recipe must have an output"), world);
		return ItemRecipe(coord, std::move(crafting_list),output);
		
	}
	//will make this an asset
	inline ItemRecipes recipe_booklet_from_path(const json::Value& value,ecs::Ecs& world) {
		stn::array<ItemRecipe> recipes;
		const json::Array& arr = value.extract<json::Array>().expect("must be an array of recipes");
		for (const stn::box<json::Value>& val:arr) {
			recipes.push(recipe_from_json(*val, world));
		}
		return ItemRecipes(recipes);
	}

}