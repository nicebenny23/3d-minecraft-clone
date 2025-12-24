#pragma once
#include "ecs.h"
#include "../transform.h"
#include "commands.h"
namespace ecs {
	struct transform_comp : component {
		Transform transform;
		transform_comp(v3::Point3 pos) {
			transform.position = pos;
		}
	};
	struct obj {

		size_t id() const {
			return ent.id();
		}
		size_t generation() const {
			return ent.generation();
		}
		entity inner() const {
			return ent;
		}
		obj() :ent(0,0), ecs(nullptr) {};
		obj(entity inner, Ecs* ecs) :ecs(ecs), ent(inner) {}
		bool operator ==(const obj& other) const{
			return ent == other.ent&&ecs==other.ecs;
		}
		
		//we live as long as we can no need to check
		template<ComponentType T>
		T& get_component() {
			return ecs->get_component<T>(ent);
		}
		template<ComponentType T>
		T& get_component_unchecked() {
			return ecs->get_component_unchecked<T>(ent);
		}
		template<ComponentType T>
		void remove_component() {
			return ecs->remove_component_unchecked(ent,ecs->components.get_id<T>());
		}
		template<ComponentType T>
		const T& get_component() const {
			return ecs->get_component<T>(ent);
		}
		template<ComponentType T>
		bool has_component() const {
			return ecs->has_component<T>(ent);
		}
		Ecs* world() {
			return ecs;
		}
		template<ComponentType T>
		stn::Option<T&> get_component_opt() {
			if (has_component<T>()) {
				return stn::Option<T&>(ecs->get_component_unchecked<T>(ent));
			}
			return stn::None;
		}
		template<ComponentType T>
		T* get_component_ptr() {
			if (has_component<T>()) {
				return &ecs->get_component_unchecked<T>(ent);
			}
			return nullptr;
		}
		template<ComponentType T, typename ...Args > requires std::constructible_from<T, Args&&...>
		T& add_component(Args&&... args) {
			return ecs->add_component<T>(ent, std::forward<Args>(args)...);
		}
		void destroy() &&{
			ecs->write_command(DestroyEntity(ent));
		}
		template<ComponentType C>
		void destroy_component() {
		ecs->write_command< DestroyComponent>(DestroyComponent(ent,ecs->get_component_id<C>()));
		}
		bool exists() const{
			return ecs->entities.is_valid(ent);
		}
	private:
		entity ent;
		Ecs* ecs;
	};

}