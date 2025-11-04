#include "component.h"
#pragma once
namespace ecs{
struct Ecs {
	Archtypes archetypes;
	Components components;
	Entities entities;
	obj spawn_empty()
	{
		entity new_entity = entities.allocate_entity();
		archetypes.add_to_empty(new_entity);
		return obj(new_entity, this);
	}
	
	bool has_component(entity ent, component_id id) const {
		if (entities.is_valid(ent))
		{
			return components.has_component(ent, id);
		}
		else
		{
			throw 
		}
	}

	template<typename T>
	bool has_component(obj object) {
		return has_component(object, components.insert_id<T>());
	}
	template<ComponentType T, typename ...Args>
	T& add_component(entity object, Args&&... args) {
		components.emplace<T>(object, std::forward<Args>(args...));
		archetypes.transfer_entity_to_flipped_index(object, components.insert_id<T>());
	}
	template<ComponentType T>
	T& get_component(entity object) {
		return components.get_component<T>(object);
	}
	template<ComponentType T>
	const T& get_component(entity object) const {
		return components.get_component<T>(object);
	}
	template<typename... Components>
	std::tuple<Components*...> get_tuple(entity obj, const stn::array<ecs::component_id>& indices) {
		return[&]<size_t... Is>(std::index_sequence<Is...>) {
			return std::tuple<Components*...>{
				(Components*)components[indices[Is]][obj]...
			};
		}(std::index_sequence_for<Components...>{});
	}

	const component& get_by_id(entity object,component_id id) const {
		return components[id][object];
	}
	void remove_component_immidiate_unchecked(entity object, component_id id) {
		
		component_storage& store = components[id].storage();
		store.destroy_hook_unchecked(object);
		store.remove_at(object);
		archetypes.transfer_entity_to_flipped_index(object,id);
	}
	void remove_object_unchecked(entity object) {
		const stn::array<component_id>& cached = archetypes.archetype_of(object).view_cached();
		for (component_id id:cached)
		{
			component_storage& store = components[id].storage();
			store.destroy_hook_unchecked(object);
			store.remove_at(object);
		}
		archetypes.remove_from_archetypes(object);
		entities.remove_entity(object);
	}
};
 