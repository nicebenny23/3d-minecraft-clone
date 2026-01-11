#include "component.h"
#pragma once
namespace ecs {
	struct Ecs;
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
	inline constexpr bool is_tuple_v = is_tuple<T>::value;
	//who knows how this one works(not mine)
	template<typename Tuple1, typename Tuple2>
	concept SameTupleType =
		is_tuple_v<Tuple1> && is_tuple_v<Tuple2> &&
		std::tuple_size_v<Tuple1> == std::tuple_size_v<Tuple2> &&
		[]<std::size_t... I>(std::index_sequence<I...>) {
		return ((std::same_as<
			std::tuple_element_t<I, Tuple1>,
			std::tuple_element_t<I, Tuple2>
		>) && ...);
	}(std::make_index_sequence<std::tuple_size_v<Tuple1>>{});

	template<RecipeType... Recipes>
	struct RecipeBuilder:Recipe {
		std::tuple<Recipes...> recipes;
		RecipeBuilder() requires (std::is_default_constructible_v<Recipes>&&...) : recipes() {

		}  
		template<typename... Rs>
			requires SameTupleType<std::tuple<std::decay_t<Rs>...>, std::tuple<Recipes...>>
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