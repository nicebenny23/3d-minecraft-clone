#include "component.h"
#pragma once
namespace ecs {
	struct component_type {
		template<ComponentType T>
		component_type(component_id mid, std::type_identity<T> type_identity) :
			pages(stn::construct_derived<component_storage<T>>()), component_id(mid), traits(ComponentTraits<T>), ecs_instance(nullptr),type_id(stn::make_type_id<T>()) {
			
		}
		bool updates() const {
			return traits.updates;
		}
		stn::type_id component_type_id() const {
			return type_id;
		}
		size_t priority() const {
			return traits.priority;
		}
		component_id id() const {
			return component_id;
		}
		template<ComponentType T>
		component_storage<T>& storage_unchecked() {
			return pages.ref_as_unchecked<component_storage<T>>();
		}
		template<ComponentType T>
		const component_storage<T>& storage_unchecked() const{
			return pages.ref_as_unchecked<component_storage<T>>();
		}
		IComponentIndexer& storage_erased() {
			return *pages;
		}
		bool has(entity_id owner) const {
			return pages->has(owner);
		}


		component& unchecked_at(entity_id entity) {
			return pages->unchecked_at(entity);
		}
		const component& unchecked_at(entity_id entity) const {
			return pages->unchecked_at(entity);
		}


		template<ComponentType T>
		T& get_as_unchecked(entity_id entity) {
			return storage_unchecked<T>().unchecked_at(entity);
		}
		template<ComponentType T>
		const T& get_as_unchecked(entity_id entity) const {
			return storage_unchecked<T>().unchecked_at(entity);
		}
		template<ComponentType T, typename...Args>
		T& emplace(entity ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {
			T& comp = storage_unchecked<T>().emplace(ent.id(), std::forward<Args>(args)...);
			comp.ent = ent;
			comp.ecs = ecs_instance;
			comp.start();
			return comp;
		}
		template<ComponentType T, typename...Args>
		stn::insertion<T&> ensure(entity ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {
			return storage_unchecked<T>().insert(ent.id(), std::forward<Args>(args)...)
				.on_insert([this, ent](T& value) {

				value.ent = ent;
				value.ecs = ecs_instance;
				value.start();
					});
		}


		void inject_ecs_instance(Ecs* instance) {
			ecs_instance = instance;
		}
	private:
		//for injecting into the type 
		Ecs* ecs_instance;
		component_id component_id;
		stn::box<IComponentIndexer> pages;
		ComponentInfo traits;
		stn::type_id type_id;
	};
	struct Components {

		stn::span<component_type> component_types() {
			return component_list.span();
		}

		size_t component_count() const {
			return component_list.length();
		}
		
		inline bool contains_id(component_id id) const {
			return component_list.contains_index(id.id);
		}

		template<ComponentType T>
		inline component_id get_id() const {
			return component_indexer.get<T>();
		}
		template<ComponentType T>
		inline stn::Option<component_id> get_id_opt() const {
			return component_indexer.get_opt<T>();
		}
		template<ComponentType T>
		inline component_id get_id_unchecked() const {
			return component_indexer.get_unchecked<T>();
		}
		//for later
		template<ComponentType ...Types>
		inline component_ids get_ids() {
			return component_ids(stn::bitset(component_indexer.get_ids<Types...>()));
		}
		template<ComponentType T>
		inline component_id insert_id() {
			stn::insertion< component_id> inserted_id = component_indexer.insert<T>();
			if (inserted_id.is_new) {
				component_list.emplace(inserted_id.value, std::type_identity<T>()).inject_ecs_instance(ecs_instance);
			}
			return inserted_id.value;
		}
		template<ComponentType ...Types>
		inline component_ids insert_ids() {
			return component_ids(stn::bitset(stn::array({ insert_id<Types>()... })));
		}

		template<ComponentType ...Types>
		inline stn::array<component_id> insert_id_list() {
			return stn::array({ insert_id<Types>()... });
		}


		component_type& operator[](component_id id) {
			return component_list[id.id];
		}
		const component_type& operator[](component_id id) const {
			return component_list[id.id];
		}
		component_type& unchecked_at(component_id id) {
			return component_list.unchecked_at(id.id);
		}
		const component_type& unchecked_at(component_id id) const {
			return component_list.unchecked_at(id.id);
		}
		template<ComponentType T, typename ...Args>
		T& emplace(entity_id id, Args&&... args) requires std::constructible_from<T, Args&&...> {
			component_id comp_id = insert_id<T>();
			return unchecked_at(comp_id).emplace<T>(id, std::forward<Args>(args)...);
		}
		template<ComponentType T, typename ...Args>
		stn::insertion<T&> ensure(entity id, Args&&... args) requires std::constructible_from<T, Args&&...> {
			component_id comp_id = insert_id<T>();
			return unchecked_at(comp_id).ensure<T>(id, std::forward<Args>(args)...);
		}
		bool has_component(entity_id object, component_id id) const {
			return (*this)[id].has(object);
		}
		template<ComponentType T>
		bool has_component(entity_id object) const {
			return has_component(object, get_id<T>());
		}

		template<ComponentType T>
		T& get_component_unchecked(entity_id entity) {
			return unchecked_at(get_id_unchecked<T>()).get_as_unchecked<T>(entity);

		}
		template<ComponentType T>
		T& get_component(entity_id entity) {
			component_type& comp_type = unchecked_at(get_id<T>());
			if (!comp_type.has(entity)) {
				stn::throw_logic_error("object does not have specified component");
			}
			return comp_type.get_as_unchecked<T>(entity);
		}

		template<ComponentType T>
		const T& get_component(entity_id entity)const {
			component_type& comp_type = unchecked_at(get_id<T>());
			if (!comp_type.has(entity)) {
				stn::throw_logic_error("object does not have specified component");
			}
			return comp_type.get_as_unchecked<T>(entity);
		}
		template<ComponentType T>
		stn::Option<T&> get_component_opt(entity_id entity) {
			stn::Option<component_id> id = get_id_opt<T>();
			if (id) {
				component_type& comp_type = unchecked_at(id.unwrap());
				if (comp_type.has(entity)) {
					return stn::Option<T&>(comp_type.get_as_unchecked<T>(entity));
				}
			}
			return stn::None;
		}

		template<ComponentType T>
		stn::Option<const T&> get_component_opt(entity_id entity) const {
			stn::Option<component_id> id = get_id_opt<T>();
			if (id) {
				const component_type& comp_type = unchecked_at(id.unwrap());
				if (comp_type.has(entity)) {
					return stn::Option<const T&>(comp_type.get_as_unchecked<T>(entity));
				}
			}
			return stn::None;
		}
		void inject_ecs_instance(Ecs* instance) {
			ecs_instance = instance;
		}
	private:
		stn::array<component_type> component_list;
		stn::type_indexer<component_id> component_indexer;
		Ecs* ecs_instance;
	};

}