#include "game_object.h"
#include "../../util/dynpool.h"
#pragma once
namespace ecs {

	struct component {

		obj owner;
		ecs::component_id comp_id;
		//called on destroy used for deallocation

		virtual void destroy_hook();
		//fix
		virtual ~component() = default;
		void destroy();
		virtual void start();
		virtual void update();
		virtual void onplayerclick();
		virtual void oncollision(obj collidedwith);
		

	};
	template<typename T>
	concept ComponentType = std::derived_from<T, component>;

	struct component_storage {
		template<ComponentType T>
		component_storage(std::type_identity<T> type_identity) :
			pool(stn::flux(std::type_identity<T>())) {
			store = array<stn::flux_token<component>>();
		}
		component& operator[](entity entity) {
			return store[entity.id()].get_unchecked();
		}
		const component& operator[](entity entity) const {
			return store[entity.id()].get_unchecked();
		}
		template<ComponentType T>
		T& get_as_unchecked(entity entity) {
			return store[entity.id()].get_unchecked_as<T>();
		}
		template<ComponentType T>
		const T& get_as_unchecked(entity entity) const{
			return store[entity.id()].get_unchecked_as<T>();
		}
		bool has(entity ent) const {
			return store.contains_index(ent.id()) && store[ent.id()];
		}
		template<typename T, typename...Args>
		component& emplace_unchecked(entity ent, Args&&... args) {
			store.reach(ent.id()) = pool.emplace_unchecked<T>(std::forward<Args>(args)...);
			return *store[ent.id()];
		}
		template<typename T, typename...Args>
		component& emplace(entity ent, Args&&... args) {
			if (has(ent))
			{
				throw std::logic_error("emplace<T> cannot overwrite an existing component");
			}
		return emplace_unchecked<T>(std::forward<Args>(args...));
		}
		void remove_at(entity entity) {
			store[entity.id()].release();
		}
		void destroy_hook_unchecked(entity entity) {
			store.reach(entity.id()).get_unchecked().destroy_hook();
		}
		
		
		
	private:
		stn::array<stn::flux_token<component>> store;
		stn::flux pool;
	};
	//component concept
	
	struct component_type {


		template<ComponentType T>
		component_type(ecs::component_id mid, std::type_identity<T> type_identity) :store(std::type_identity<T>()), component_id(mid) {}
		component_id id() const {
			return component_id;
		}
		component_storage& storage() {
			return store;
		}
		component& operator[](entity entity) {
			return store[entity];
		}

		const component& operator[](entity entity) const {
			return store[entity];
		}

		template<ComponentType T,typename...Args>
		component& emplace(entity ent,Args&&... args) {
			component& comp = store.emplace<T>(ent,std::forward<Args>(args)...);
			comp.comp_id = id();
			comp.owner = ent;
			return comp;
		}
		bool has(entity owner) const{
			return store.has(owner);
		}
		
	private:
		ecs::component_id component_id;
		component_storage store;


	};
	struct Components {
		stn::array<component_type> component_list;
		type_map::type_indexer<component_id> component_indexer;
		template<ComponentType T>
		inline component_id get_id() {
			return component_indexer.get_id<T>();
		}
		//for later
		template<ComponentType ...Types>
		inline component_ids get_ids() {
			return component_ids(stn::bitset(component_indexer.get_type_ids<Types...>()));
		}
		template<ComponentType T>
		inline component_id insert_id() {
			auto& [is_new,id] = component_indexer.insert<T>();
			if (is_new)
			{
				component_list.emplace(id, std::type_identity<T>());
			}
			return id;
		}
		size_t component_types() const{
			return component_list.length();
		}
		component_type& operator[](component_id id) {
			return component_list[id.id];
		}
		const component_type& operator[](component_id id) const {
			return component_list[id.id];
		}
		template<ComponentType T,typename ...Args>
		component& emplace(entity id, Args&&... args){
			component_id index = insert_id<T>();
			return (*this)[insert<T>()].emplace<T>(std::forward<Args>(args)...);
		}

		bool has_component(entity object,component_id id) const {
			return (*this)[id].has(object);
		}
		template<ComponentType T>
		bool has_component(entity object) const{
			return has_component(object,insert_id<T>());
		}
		template<ComponentType T> 
		T& get_component(entity entity){
			component_type& comp_type= (*this)[get_id<T>()];
			if (comp_type.has(entity))
			{
				return comp_type.store.get_as_unchecked<T>();
			}
			else
			{
				throw std::logic_error("odk");
			}
		}
		template<ComponentType T>
		const T& get_component(entity entity)const {
			component_type& comp_type = (*this)[get_id<T>()];
			if (comp_type.has(entity))
			{
				return comp_type.store.get_as_unchecked<T>();
			}
			else
			{
				throw std::logic_error("odk");
			}
		}
	};




};