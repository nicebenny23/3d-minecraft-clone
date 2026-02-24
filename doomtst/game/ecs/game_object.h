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
			return ecs->get_component_unchecked<T>(ent.id());
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
		template<ComponentType ...Types>
		bool has_components() const {
			return ecs->has_components<Types...>(ent);
		}
		Ecs& world() const {
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
				return &ecs->get_component_unchecked<T>(ent.id());
			}
			return nullptr;
		}
		template<ComponentType T, typename ...Args > requires std::constructible_from<T, Args&&...>
		T& add_component(Args&&... args) {
			return ecs->add_component<T>(ent, std::forward<Args>(args)...);
		}
		template<ComponentType T, typename ...Args > requires std::constructible_from<T, Args&&...>
		T& set_emplace_component(Args&&... args) {
			return ecs->set_component<T>(ent, std::forward<Args>(args)...);
		}
		void destroy() {
			ecs->destroy_object(ent);
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
			add_component<Parent>().add_child(child.inner());
		}

		template<RecipeType T>
		void apply_recipe(const T& recipe) {
			ecs::apply(recipe, *this);
		}
		template<RecipeType T, typename ...Args> requires std::constructible_from<T, Args&&...>
		ecs::obj spawn_child(Args&&... args);
		
	private:
		Ecs* ecs;
		entity ent;
};

	template<RecipeType T, typename ...Args> requires std::constructible_from<T, Args&&...>
	obj spawn_emplaced(ecs::Ecs& world, Args&&... args) {
		ecs::obj object = world.spawn_empty();
		ecs::apply(T(std::forward<Args>(args)...),object);
		return object;
	}

	template<RecipeType T>
	obj spawn(ecs::Ecs& world, T&& spawner) {
		ecs::obj object = world.spawn_empty();
		ecs::apply(std::forward<T>(spawner),object);
		return object;
	}
	template<RecipeType T, typename ...Args> requires std::constructible_from<T, Args&&...>
	inline obj obj::spawn_child(Args && ...args) {
		ecs::obj child = spawn_emplaced<T>(world(), std::forward<Args>(args)...);
		add_child(child);
		return child;
	}
	template<typename T>
	concept ObjectLike = std::same_as<T, obj> || requires(const T & value) {
		{
			value.object()
		}->std::same_as<ecs::obj>;
	};
	template<ObjectLike T>
	ecs::obj as_object(const T& object) {
		if constexpr (std::same_as<T,obj>) {
			return object;
		}
		else {
			return object.object();
		}
	}

}