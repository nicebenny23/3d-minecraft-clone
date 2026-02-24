#include "component.h"
#pragma once
namespace ecs {
	struct ecs::obj;

	template<typename T>
	concept RecipeType = requires(const T & spawner, ecs::obj & object) {
		{
			spawner.apply(object)
		}->std::same_as<void>;
	}||stn::void_invokable<T&,ecs::obj&>;
	template<typename T>
	struct is_tuple : std::false_type {
	};

	template<typename... Ts>
	struct is_tuple<std::tuple<Ts...>> : std::true_type {
	};
	template<RecipeType T>
	void apply(const T& recipe, ecs::obj& object) {
		if constexpr(stn::void_invokable<const T&, ecs::obj&>) {
			recipe(object);
		}
		else {
			recipe.apply(object);
		}
	}


	//did this 
	template<typename T>
	concept TupleType = is_tuple<T>::value;
	template<RecipeType... Recipes>
	struct RecipeBuilder{
		std::tuple<Recipes...> recipes;
		RecipeBuilder() requires (std::is_default_constructible_v<Recipes>&&...) : recipes() {

		}  
		template<typename... Rs>
			requires std::same_as<std::tuple<std::decay_t<Rs>...>, std::tuple<Recipes...>>
		RecipeBuilder(Rs&&... rs) : recipes(std::forward<Rs>(rs)...) {
		}
		void apply(ecs::obj& object) const{
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