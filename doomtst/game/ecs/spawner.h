#include "game_object.h"
#pragma once
namespace ecs {

	template<typename Spawner>
	concept SpawnerType =
		requires(Spawner spawner, ecs::obj object) {
			{
				spawner.apply(object)
			} -> std::same_as<void>;
	};

	template<SpawnerType Spawner,typename ...Args> requires std::constructible_from<Spawner,Args&&...>
	ecs::obj spawn_emplaced(ecs::Ecs& world, Args&&... args) {
		ecs::obj object= world.spawn_empty();
		Spawner(std::forward<Args>(args)...).apply(object);
		return object;

	}
	template<SpawnerType Spawner>
	ecs::obj spawn(ecs::Ecs& world, Spawner&& spawner) {
		ecs::obj object = world.spawn_empty();
		std::forward<Spawner>(spawner).apply(object);
		return object;
	}
}