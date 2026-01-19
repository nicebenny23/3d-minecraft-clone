#include "component.h"
#pragma once
namespace ecs {
	struct ecs::obj;
	struct Recipe{
		virtual void apply(ecs::obj& object) = 0;
	};

	template<typename T>
	concept RecipeType = std::derived_from<T, Recipe>;
	template<typename T>
	struct is_tuple : std::false_type {
	};

	template<typename... Ts>
	struct is_tuple<std::tuple<Ts...>> : std::true_type {
	};

	//did this 
	template<typename T>
	concept TupleType = is_tuple<T>::value;
	template<RecipeType... Recipes>
	struct RecipeBuilder:Recipe {
		std::tuple<Recipes...> recipes;
		RecipeBuilder() requires (std::is_default_constructible_v<Recipes>&&...) : recipes() {

		}  
		template<typename... Rs>
			requires std::same_as<std::tuple<std::decay_t<Rs>...>, std::tuple<Recipes...>>
		RecipeBuilder(Rs&&... rs) : recipes(std::forward<Rs>(rs)...) {
		}
		void apply(ecs::obj& object) {
			std::apply([&](auto&... r) { (r.apply(object), ...); }, recipes);
		}
		template<RecipeType T>
		RecipeBuilder<Recipes..., T> then(T&& recipe) {
			return std::apply([&](auto&&... old) {return RecipeBuilder<Recipes..., T>(std::forward<decltype(old)>(old)..., std::forward<T>(recipe));}, recipes);
		}
		template<RecipeType T,typename ...Args> requires std::constructible_from<T,Args&&...>
		RecipeBuilder<Recipes..., T> then(Args&&... args) {
			return std::apply([&](auto&&... old) {return RecipeBuilder<Recipes..., T>(std::forward<decltype(old)>(old)..., T(std::forward<Args>(args)...)); },recipes);
		}
	};

}