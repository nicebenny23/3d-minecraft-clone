#include "component.h"
#include "game_object.h"
"
#pragma once
namespace ecs {
	
	struct World {
		obj spawn_empty()
		{
			return ecs.spawn_empty();
		}
		template<typename T>
		bool has_component(obj object,component_id id) {
			return archetypes[entities[object.inner()].arch_id()].has_component(id);
		}
	private:
		Ecs ecs;
	};

}