#pragma once
#include "component.h"
#include "command_buffer.h"
#include "System.h"
namespace ecs {
	struct Ecs {

		struct run_updates :System {
			run_updates() {

			};
			void run(Ecs& ecs) {

				stn::array<component_type*> types = ecs.components.component_types()
					.map([](auto&& comp_type) {return &comp_type; })
					.filter([](component_type* type) {return type->updates(); })
					.into<stn::array>();

				types | stn::sort([](component_type* a, component_type* b) {
					return a->priority() > b->priority();
					});

				for (component_type* mgr : types) {
					for (Archetype& archetype : stn::range(ecs.archetypes).filter([&](auto&& archetype) {return archetype.has_component(mgr->id()); })) {
						// 1) Snapshot the count _once_
						size_t originalCount = archetype.count();
						for (size_t i = 0; i < originalCount; ++i) {
							(*mgr)[archetype[i]].update();
						}
					}
				}


			}

		};
		Archetypes archetypes;
		Components components;
		Entities entities;
		Systems systems;
		Commands commands;
		template<CommandType cmd>
		void insert_command(cmd&& command) {
			commands.push(std::forward<cmd>(command));
		}
		void run_commands() {
			commands.run_on(*this);
		}
		template<SystemType Sys>
		void insert_system(Sys& sys) {
			systems.insert(sys);
		}
		void run_systems() {
			systems.run_on(*this);
		}
		obj spawn_empty();
		Ecs(size_t total_entities) :entities(total_entities), archetypes(total_entities), commands(),systems(){
			components.inject_ecs_instance(this);
			systems.emplace<run_updates>();
		}

		bool contains(space_id ent) const {
			return entities.is_valid(ent);
		}
		bool has_component(space_id ent, component_id id) const {
			entities.assert_valid(ent);
			return components.has_component(ent, id);
		}

		template<typename T>
		bool has_component(space_id object) {
			entities.assert_valid(object);
			return has_component(object, components.insert_id<T>());
		}
		template<ComponentType T, typename ...Args>
		T& add_component(space_id object, Args&&... args) {
			entities.assert_valid(object);
			T& component = components.emplace<T>(object, std::forward<Args>(args)...);
			archetypes.transfer_entity_to_flipped_index(object, components.insert_id<T>());
			return component;
		}
		template<ComponentType T>
		T& get_component(space_id object) {
			entities.assert_valid(object);
			return components.get_component<T>(object);
		}
		template<ComponentType T>
		const T& get_component(space_id object) const {
			entities.assert_valid(object);
			return components.get_component<T>(object);
		}
		template<typename... Components>
		std::tuple<Components&...> get_tuple_unchecked(space_id obj, const stn::array<component_id>& indices) {
			return[&]<size_t... Is>(std::index_sequence<Is...>) {
				return std::tuple<Components&...>{
					(Components&)components[indices[Is]][obj]...
				};
			}(std::index_sequence_for<Components...>{});
		}

		const component& get_by_id(space_id object, component_id id) const {
			entities.assert_valid(object);
			return components[id][object];
		}
		template<ComponentType T>
		component_id insert_component_id() const
		{
			return components.insert_id<T>();
		}
	
		template<ComponentType T>
		component_id get_component_id() const
		{
			return components.get_id<T>();
		}
		void remove_component_unchecked(space_id object, component_id id) {

			component_storage& store = components[id].storage();
			store.remove_at(object);
			archetypes.transfer_entity_to_flipped_index(object, id);
		}
		void remove_object_unchecked(space_id object) {
			const stn::array<component_id>& cached = archetypes.archetype_of(object).view_cached();
			for (component_id id : cached)
			{
			components[id].storage().remove_at(object);
			}

			//removes from both at the same time
			archetypes.remove_from_archetypes(object);
			entities.remove_space_id(object);
		}
	};


}