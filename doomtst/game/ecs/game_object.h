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
				return ecs->entities.is_valid(ent);
			}
			return false;
		}
	private:
		entity ent;
		Ecs* ecs;
	};
	struct object_handle {

		object_handle()=delete;
		object_handle(object_handle&& other) noexcept :owned(other.owned){
			other.owned = obj();
		}

		object_handle& operator=(object_handle&& other) noexcept {
			if (this != &other) {
				
				owned.destroy();
				owned = other.owned;
				other.owned = obj();
			}
			return *this;
		}

		object_handle(const object_handle&) = delete;
		object_handle& operator=(const object_handle&) = delete;
		~object_handle() {
			if (owned.exists()) {
				owned.destroy();
			}
		}
		const obj get() const {
			return owned;
		}

		obj get() {
			return owned;
		}

		object_handle(obj o) : owned(o) {
		}

		size_t id() const {
			return owned.id();
		}
		size_t generation() const {
			return owned.generation();
		}
		entity inner() const {
			return owned.inner();
		}

		bool operator==(const object_handle& other) const {
			return owned == other.owned;
		}

		template<ComponentType T>
		T& get_component() {
			return owned.get_component<T>();
		}

		template<ComponentType T>
		T& get_component_unchecked() {
			return owned.get_component_unchecked<T>();
		}

		template<ComponentType T>
		const T& get_component() const {
			return owned.get_component<T>();
		}

		template<ComponentType T>
		bool has_component() const {
			return owned.has_component<T>();
		}

		template<ComponentType T>
		stn::Option<T&> get_component_opt() {
			return owned.get_component_opt<T>();
		}

		template<ComponentType T>
		T* get_component_ptr() {
			return owned.get_component_ptr<T>();
		}

		template<ComponentType T, typename ...Args>
			requires std::constructible_from<T, Args&&...>
		T& add_component(Args&&... args) {
			return owned.add_component<T>(std::forward<Args>(args)...);
		}

		template<ComponentType T, typename ...Args>
			requires std::constructible_from<T, Args&&...>
		T& ensure_component(Args&&... args) {
			return owned.ensure_component<T>(std::forward<Args>(args)...);
		}

		Ecs& world() {
			return owned.world();
		}

		bool exists() const {
			return owned.exists();
		}
	private:

		obj owned;
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
}