#pragma once
#include "ecs.h"
#include "commands.h"
namespace ecs {

	struct obj {

		size_t id() const {
			return ent.id();
		}
		size_t generation() const {
			return ent.generation();
		}
		space_id inner() const {
			return ent;
		}
		obj(space_id inner, Ecs* ecs) :ecs(ecs), ent(inner) {}
		template<ComponentType T>
		T& get_component() {
			return ecs->get_component<T>(ent);
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

		template<ComponentType T>
		T* get_component_ptr() {
			if (!has_component<T>())
			{
				return nullptr;
			}
			return &get_component<T>();
		}
		template<typename T, typename ...Args>
		const T& add_component(Args&&... args) const {
			return ecs->add_component<T>(ent, std::forward<Args>(args)...);
		}
		void destroy() {
		//	ecs->insert_command(DestroyEntity(ent));
		}
		template<ComponentType C>
		void destroy_component() {
		ecs->write_command< DestroyComponent>(DestroyComponent(ent,ecs->get_component_id<C>()));
		}
	private:
		space_id ent;
		Ecs* ecs;
	};

}