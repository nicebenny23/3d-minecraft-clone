#include "component.h"
#pragma once
namespace ecs {
	
	struct World {
		Archtypes archetypes;
		Components components;
		Entities entities;

		obj spawn_empty()
		{
			entity new_entity= entities.allocate_entity();
			archetypes.add_to_empty(new_entity);
			return obj(new_entity,this);
		}
		template<typename T>
		bool has_component(obj object,component_id id) {
			return archetypes[entities[object.inner()].arch_id()].has_component(id);
		}
	};

}