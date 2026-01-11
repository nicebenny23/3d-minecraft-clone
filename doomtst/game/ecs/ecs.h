#pragma once
#include "component.h"
#include "resources.h"
#include "ecs_commands.h"
#include "System.h"
#include "commands.h"
#include "message.h"
#include "archtype.h"
#include "../../assets/Assets.h"
#include "spawner.h"
namespace ecs {
	struct Ecs {
		Ecs(std::uint32_t total_entities) :entities(total_entities), archetypes(total_entities){
			components.inject_ecs_instance(this);
			ensure_resource<Systems>();
			emplace_system<run_updates>();
			emplace_system<delete_objects>();
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
						uint32_t originalCount = archetype.count();
						for (uint32_t i = 0; i < originalCount; ++i) {
							mgr->unchecked_at(archetype[archetype_index(i)]).update();
						}
					}
				}
			}

		};
		Archetypes archetypes;
		Components components;
		Entities entities;
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
			return resources.insert<T>(std::forward<Args>(args)...);
		}
		template<ResourceType T>
		T& ensure_resource() {
			return resources.ensure<T>();
		}
		template<ResourceType T>
		void remove_resource() {
			return resources.remove<T>();
		}
		void clear_resources() {
			return resources.clear();
		}

		
		template<assets::LoadDescriptorType T>
		stn::Option<assets::AssetHandle<assets::DescriptorAssetType<T>>> load_asset(const T& descriptor) {
			return ensure_resource<assets::Assets>().load(descriptor);
		}
		
		template<assets::LoadDescriptorType T,typename ...Args> requires std::constructible_from<T,Args&&...>
		stn::Option<assets::AssetHandle<assets::DescriptorAssetType<T>>> load_asset_emplaced(Args&&... args ) {
			return load_asset<T>(T(std::forward<Args>(args)...));
		}

		template<assets::LoaderType T, typename ...Args> 
			requires std::constructible_from<T, Args&&...>
		void emplace_asset_loader(Args&&... args) {
			return ensure_resource<assets::Assets>().emplace_loader<T>(std::forward<Args>(args)...);
		}

		template<assets::AssetType T> 
		stn::Option<assets::AssetHandle<T>> from_name(std::string name) {
			return ensure_resource<assets::Assets>().from_name<T>(name);
		}

		template<typename T>
		Events<T>& events() {
			return ensure_resource<Events<T>>();
		}
		template<typename T, typename ...Args>
		void write_event(Args&&... args) requires std::constructible_from<T, Args&&...> {
			return events<T>().write(std::forward<Args>(args)...);
		}
		template<typename T>
		EventReader<T> make_reader() {
			return events<T>().make_reader();
		}

		
		template<typename T>
		Commands<T>& commands() {
			return ensure_resource<Commands<T>>();
		}
		template<typename T>
		void write_command(T&& command) {
			commands<T>().emplace(std::forward<T>(command));
		}
		template<typename T, typename ...Args> requires std::constructible_from<T,Args&&...>
		void write_command(Args&&... args) {
			commands<T>().emplace(std::forward<Args>(args)...);
		}
		template<typename T>
		decltype(auto) read_commands() {
			return commands<T>().read();
		}
		
		const Systems& systems() const {
			return get_resource<Systems>().unwrap();
		}

		Systems& systems() {
		return get_resource<Systems>().unwrap();
		}
		template<SystemType Sys>
		void insert_system(Sys& sys) {
			systems().insert(sys);
		}
		template<SystemType T, typename...Args>
		void emplace_system(Args&&... args) {
			systems().emplace<T>(std::forward<Args>(args)...);
		}
		void run_systems() {
			systems().run_on(*this);
		}


		template<ComponentType T>
		component_id insert_component_id() const {
			return components.insert_id<T>();
		}

		template<ComponentType T>
		component_id get_component_id() const {
			return components.get_id<T>();
		}


		obj spawn_empty();

		bool contains(entity ent) const {
			return entities.is_valid(ent);
		}
		template<ComponentType T, typename ...Args>
		T& ensure_component(entity object, Args&&... args)   requires std::constructible_from<T, Args&&...> {
			entities.assert_valid(object);
			return components.ensure<T>(object, std::forward<Args>(args)...)
				.on_insert([this, object](T& component) {
				archetypes.transfer_entity_to_flipped_index(object, components.get_id_unchecked<T>());
				}).value;
		}
		template<ComponentType T, typename ...Args>
		T& add_component(entity object, Args&&... args)   requires std::constructible_from<T, Args&&...> {
			entities.assert_valid(object);
			T& component = components.emplace<T>(object, std::forward<Args>(args)...);
			archetypes.transfer_entity_to_flipped_index(object, components.insert_id<T>());
			return component;
		}
		
		bool has_component(entity ent, component_id id) const {
			entities.assert_valid(ent);
			return components.has_component(ent, id);
		}

		template<typename T>
		bool has_component(entity object) {
			return has_component(object, components.insert_id<T>());
		}
		template<ComponentType... Components>
		bool has_components(entity object) {
			entities.assert_valid(object);
			return archetypes.archetype_of(object).has_components(components.get_ids<Components...>());
		}

		template<ComponentType T>
		T& get_component_unchecked(entity object) {
			return components.get_component_unchecked<T>(object);
		}

		template<ComponentType T>
		T& get_component(entity object) {
			entities.assert_valid(object);
			return components.get_component<T>(object);
		}
		template<ComponentType T>
		const T& get_component(entity object) const {
			entities.assert_valid(object);
			return components.get_component<T>(object);
		}
		template<ComponentType T>
		stn::Option<T&> get_component_opt(entity object) {
			if (entities.is_valid(object)) {
				return components.get_component_opt<T>(object);
			}
			return stn::None;
		}
		template<ComponentType T>
		stn::Option<const T&> get_component_opt(entity object) const{
			if (entities.is_valid(object)) {
				return stn::Option<const T&>(components.get_component_opt<T>(object));
			}
			return stn::None;
		}
		template<ComponentType... Components>
		std::tuple<Components&...> get_tuple_unchecked(entity obj,const stn::span<const component_id>& indices) {
			
			return[&]<size_t... Is>(std::index_sequence<Is...>) {
				return std::tuple<Components&...>{
					components.unchecked_at(indices.unchecked_at(Is)).get_as_unchecked<Components>(obj)...
				};
			}(std::index_sequence_for<Components...>{});
		}

		
		void remove_component_unchecked(entity object, component_id id) {

			component_storage& pages = components.unchecked_at(id).storage();
			pages.remove_at_unchecked(object);
			archetypes.transfer_entity_to_flipped_index(object, id);
		}
		void remove_object_unchecked(entity object) {
			stn::span<const component_id> cached = archetypes.archetype_of(object).view_cached();
			for (component_id id : cached) {
				components.unchecked_at(id).storage().remove_at_unchecked(object);
			}

			//removes from both at the same time
			archetypes.remove_from_archetypes(object);
			entities.remove_entity(object);
		}

	};

}