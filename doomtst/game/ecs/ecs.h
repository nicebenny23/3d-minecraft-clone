#pragma once
#include "component.h"
#include "resources.h"
#include "ecs_commands.h"
#include "System.h"
#include "commands.h"
#include "message.h"
namespace ecs {
	struct Ecs {
		Ecs(size_t total_entities) :entities(total_entities), archetypes(total_entities), systems() {
			components.inject_ecs_instance(this);
			systems.emplace<run_updates>();
			systems.emplace<delete_objects>();
		}

		struct delete_objects :System {
			using DependencyTag = Depends::dependency_last;
			delete_objects() {
			};

			void run(Ecs& ecs) {
				for (DestroyEntity& entity_cmd : ecs.read_commands<DestroyEntity>()) {
					if (ecs.entities.is_valid(entity_cmd.ent)) {
						ecs.remove_object_unchecked(entity_cmd.ent);
					}
				}
				for (DestroyComponent& component_cmd : ecs.read_commands<DestroyComponent>()) {
					if (ecs.entities.is_valid(component_cmd.owning_entity)) {
						if (ecs.has_component(component_cmd.owning_entity, component_cmd.id)) {
							return;
						}
						ecs.remove_component_unchecked(component_cmd.owning_entity, component_cmd.id);
					}
				}
			}

		};
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
		Resources resources;
		template<ResourceType T>
		stn::Option<T&> get_resource() {
			return resources.get<T>();
		}
		template<ResourceType T>
		stn::Option<const T&> get_resource() const {
			return resources.get<T>();
		}
		template<ResourceType T, typename ...Args>
		T& insert_resource(Args&&... args) requires std::constructible_from<T, Args&&...> {
			return resources.insert<T>(std::forward<Args>(args));
		}
		template<ResourceType T>
		T& get_resource_or_default() {
			return resources.get_or_default<T>();
		}
		template<ResourceType T>
		void remove_resource() {
			return resources.remove<T>();
		}
		void clear_resources() {
			return resources.clear();
		}
		template<typename T>
		Events<T>& events() {
			return get_resource_or_default<Events<T>>();
		}
		template<typename T, typename ...Args>
		void write_event(Args&&... args) {
			return events<T>().write(std::forward<Args>(args)...);
		}
		template<typename T>
		EventReader<T> make_reader() {
			return events<T>().make_reader();
		}

		template<typename T>
		Commands<T>& commands() {
			return get_resource_or_default<Commands<T>>();
		}
		template<typename T>
		void write_command(T&& command) {
			commands<T>().write(std::forward<T>(command));
		}
		template<typename T>
		decltype(auto) read_commands() {
			return commands<T>().read();
		}
		template<SystemType Sys>
		void insert_system(Sys& sys) {
			systems.insert(sys);
		}
		void run_systems() {
			systems.run_on(*this);
		}

		obj spawn_empty();

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
		component_id insert_component_id() const {
			return components.insert_id<T>();
		}

		template<ComponentType T>
		component_id get_component_id() const {
			return components.get_id<T>();
		}
		void remove_component_unchecked(space_id object, component_id id) {

			component_storage& store = components[id].storage();
			store.remove_at(object);
			archetypes.transfer_entity_to_flipped_index(object, id);
		}
		void remove_object_unchecked(space_id object) {
			stn::span<const component_id> cached = archetypes.archetype_of(object).view_cached();
			for (component_id id : cached) {
				components[id].storage().remove_at(object);
			}

			//removes from both at the same time
			archetypes.remove_from_archetypes(object);
			entities.remove_space_id(object);
		}

	};


}