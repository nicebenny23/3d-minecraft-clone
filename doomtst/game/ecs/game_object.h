#pragma once
#include "ecs.h"
#include "../../math/transform.h"
#include "commands.h"
#include "Parent.h"
namespace ecs {
	struct transform_comp : component {
		math::Transform transform;
		transform_comp(v3::Point3 pos) {
			transform.position = pos;
		}
	};
	struct obj {

		entity_id id() const {
			return ent.id();
		}
		size_t generation() const {
			return ent.generation();
		}
		entity inner() const {
			return ent;
		}
		obj() :ent(entity_id(0),0), ecs(nullptr) {};
		obj(entity inner, Ecs& ecs) :ecs(&ecs), ent(inner) {}
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
		Ecs& world() {
			return *ecs;
		}
		template<ComponentType T>
		stn::Option<T&> get_component_opt() {
			return ecs->get_component_opt<T>(ent);
		}
		template<ComponentType T>
		stn::Option<const T&> get_component_opt() const{
			return ecs->get_component_opt<T>(ent);
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
		template<ComponentType T, typename ...Args > requires std::constructible_from<T, Args&&...>
		T& ensure_component(Args&&... args) {
			return ecs->ensure_component<T>(ent, std::forward<Args>(args)...);
		}
		void destroy() {
			ecs->write_command(DestroyEntity(ent));
		}
		template<ComponentType C>
		void destroy_component() {
		ecs->write_command< DestroyComponent>(DestroyComponent(ent,ecs->get_component_id<C>()));
		}
		bool exists() const{
			if (ecs) {
				return ecs->contains(ent);
			}
			return false;
		}
		bool dead() const {
			if (ecs) {
				return !ecs->contains(ent);
			}
			return true;
		}
		void add_child(ecs::obj child) {
			ensure_component<Parent>().add_child(child.inner());
		}
		template<RecipeType T, typename ...Args> requires std::constructible_from<T, Args&&...>
		ecs::obj spawn_child(Args&&... args);
		
	private:
		entity ent;
		Ecs* ecs;
	};

	template<RecipeType T, typename ...Args> requires std::constructible_from<T, Args&&...>
	obj spawn_emplaced(ecs::Ecs& world, Args&&... args) {
		ecs::obj object = world.spawn_empty();
		T(std::forward<Args>(args)...).apply(object);
		return object;
	}

	template<RecipeType T>
	obj spawn(ecs::Ecs& world, T&& spawner) {
		ecs::obj object = world.spawn_empty();
		std::forward<T>(spawner).apply(object);
		return object;
	}
	template<RecipeType T, typename ...Args> requires std::constructible_from<T, Args&&...>
	inline obj obj::spawn_child(Args && ...args) {
		ecs::obj child = spawn_emplaced<T>(world(), std::forward<Args>(args)...);
		add_child(child);
		return child;
	}

	

}