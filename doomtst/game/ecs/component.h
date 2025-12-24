#include "archtype.h"
#include "entities.h"
#include "component_pages.h"
#include "../../util/dynpool.h"
#include "component_id.h"
#include "../../util/exception.h"
#pragma once
namespace ecs {
	struct obj;
	struct Ecs;
	struct component {

		obj owner();
		//called on clear used for deallocation
		component() :ecs(nullptr), comp_id(0) {

		}

		virtual ~component() = default;


		virtual void start() {
		}
		virtual void update() {
		}

		component_id id() const {
			return comp_id;
		}
		virtual void destroy_hook() {
		};

	private:
		Ecs* ecs;
		component_id comp_id;
		entity ent;
		friend struct component_storage;
		friend struct component_type;
	};
	template<typename T>
	concept ComponentType = std::derived_from<T, component>;

	struct ComponentInfo {
		int priority = 0;
		bool updates = true;
	};
	template<typename T>
	inline constexpr ComponentInfo ComponentTraits{};

	struct component_storage {

		template<ComponentType T>
		component_storage(std::type_identity<T> type_identity) :
			pool(stn::flux(std::type_identity<T>())),
			store() {

		}
		component_storage& operator=(component_storage&& other) noexcept {
			if (this != &other) {
				clear();
				store = std::move(other.store);
				pool = std::move(other.pool);
			}
			return *this;
		}

		component_storage(component_storage&& other) noexcept
			: store(std::move(other.store)),
			pool(std::move(other.pool)) {
		}

		bool has(entity ent) const {
			return store.contains(ent.id());
		}


		component& unchecked_at(entity entity) {
			return store.unchecked_at(entity.id()).get_unchecked();
		}
		const component& unchecked_at(entity entity) const {
			return store.unchecked_at(entity.id()).get_unchecked();
		}
		template<ComponentType T>
		T& get_as_unchecked(entity entity) {
			return store.unchecked_at(entity.id()).get_unchecked_as<T>();
		}
		template<ComponentType T>
		const T& get_as_unchecked(entity entity) const {
			return store.unchecked_at(entity.id()).get_unchecked_as<T>();
		}


		template<typename T, typename... Args>
		T& emplace_unchecked(entity ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {
			// Direct initialization to satisfy MSVC parsing rules
			stn::flux_token<T> token(pool.template emplace_unchecked<T>(std::forward<Args>(args)...));
			T& ref = token.get_unchecked();
			store.insert_at_unchecked(ent.id(), token.template abstract<component>());

			return ref;
		}
		template<typename T, typename...Args>
		T& emplace(entity ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {
			if (has(ent)) {
				throw std::logic_error("emplace<T> cannot overwrite an existing component");
			}
			return emplace_unchecked<T>(ent, std::forward<Args>(args)...);
		}


		void remove_at_unchecked(entity ent) {
			store[ent.id()].get_unchecked().destroy_hook();
			store.remove_at_unchecked(ent.id());
		}
		void remove_at(entity ent) {
			if (has(ent)) {
				throw std::logic_error("emplace<T> cannot overwrite an existing component");
			}
			remove_at_unchecked(ent);
		}


		void clear() {
			store.clear();
			pool.clear();
		}
		~component_storage() {
			clear();
		}
	private:
		component_pages<stn::flux_token<component>, 10> store;
		stn::flux pool;
	};
	//component concept

	struct component_type {


		template<ComponentType T>
		component_type(component_id mid, std::type_identity<T> type_identity) :store(std::type_identity<T>()), component_id(mid), traits(ComponentTraits<T>), ecs_instance(nullptr) {
		}
		bool updates() const {
			return traits.updates;
		}

		size_t priority() const {
			return traits.priority;
		}
		component_id id() const {
			return component_id;
		}
		component_storage& storage() {
			return store;
		}

		bool has(entity owner) const {
			return store.has(owner);
		}


		component& unchecked_at(entity entity) {
			return store.unchecked_at(entity);
		}
		const component& unchecked_at(entity entity) const {
			return store.unchecked_at(entity);
		}


		template<ComponentType T>
		T& get_as_unchecked(entity entity) {
			return store.get_as_unchecked<T>(entity);
		}
		template<ComponentType T>
		const T& get_as_unchecked(entity entity) const {
			return store.get_as_unchecked<T>(entity);
		}
		template<ComponentType T, typename...Args>
		T& emplace(entity ent, Args&&... args)  requires std::constructible_from<T, Args&&...> {
			T& comp = store.emplace<T>(ent, std::forward<Args>(args)...);
			comp.comp_id = id();
			comp.ent = ent;
			comp.ecs = ecs_instance;
			comp.start();
			return comp;
		}

		
		void inject_ecs_instance(Ecs* instance) {
			ecs_instance = instance;
		}
	private:
		//for injecting into the type 
		Ecs* ecs_instance;
		component_id component_id;
		component_storage store;
		ComponentInfo traits;
	};
	struct Components {
		
		stn::span<component_type> component_types() {
			return component_list.span();
		}

		size_t component_count() const {
			return component_list.length();
		}


		template<ComponentType T>
		inline component_id get_id() const {
			return component_indexer.get<T>();
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
			auto [id, is_new] = component_indexer.insert<T>();
			if (is_new) {
				component_list.emplace(id, std::type_identity<T>()).inject_ecs_instance(ecs_instance);
			}
			return id;
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
		T& emplace(entity id, Args&&... args) requires std::constructible_from<T, Args&&...> {
			component_id comp_id = insert_id<T>();
			return unchecked_at(comp_id).emplace<T>(id, std::forward<Args>(args)...);
		}

		bool has_component(entity object, component_id id) const {
			return (*this)[id].has(object);
		}
		template<ComponentType T>
		bool has_component(entity object) const {
			return has_component(object, get_id<T>());
		}

		template<ComponentType T>
		T& get_component_unchecked(entity entity) {
			return unchecked_at(get_id_unchecked<T>()).get_as_unchecked<T>(entity);

		}
		template<ComponentType T>
		T& get_component(entity entity) {
			component_type& comp_type = unchecked_at(get_id<T>());
			if (!comp_type.has(entity)) {
				stn::throw_logic_error("object does not have specified component");
			}
			return comp_type.get_as_unchecked<T>(entity);
		}
		
		template<ComponentType T>
		const T& get_component(entity entity)const {
			component_type& comp_type = unchecked_at(get_id<T>());
			if (!comp_type.has(entity)) {
				stn::throw_logic_error("object does not have specified component");
			}
			return comp_type.get_as_unchecked<T>(entity);
		}

		void inject_ecs_instance(Ecs* instance) {
			ecs_instance = instance;
		}
	private:
		stn::array<component_type> component_list;
		stn::type_indexer<component_id> component_indexer;
		Ecs* ecs_instance;
	};


};