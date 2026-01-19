#pragma once
#include "game_object.h"
namespace ecs {
	struct object_handle {

		object_handle() = delete;
		object_handle(object_handle&& other) noexcept :owned(other.owned) {
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
}
