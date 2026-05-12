#include "game_object.h"
#include "../ecs/filtered_object.h"
#pragma once
namespace ecs {
	template<ComponentType... Types>
	struct WeakConstrained{
		using ConstrainedType = ecs::Constrained<Types...>;
		template<ComponentType T> requires stn::OneOf<T,Types...>
		auto get(this auto&& self) -> stn::Option<stn::apply_const_like_t<decltype(self), T>&> {
			if (alive()) {
				return element.unwrap().get_component_unchecked<T>();
			}
			return stn::None;
		}
		auto constrained(this auto&& self) -> stn::Option<stn::apply_const_like_t<decltype(self), ConstrainedType>&>
		{
			if (alive()) {
				return element;
			}
			return stn::None;

		}
		stn::Option<ecs::obj> object() const {
			if (alive()) {
				return element.unwrap().object();
			}
			return stn::None;

		}
		WeakConstrained() :element() {

		}
		WeakConstrained(ecs::obj object) :element(object) {

		}
		bool alive() const {
			element.retain(element.unwrap().is_valid);
			return element.is_some();
		}
		explicit operator bool() const {
			return alive();
		}
		void set(ConstrainedType object) {
			element = object;
		}

	private:
		mutable stn::Option<ConstrainedType> element;
	};
	struct WeakObject {
		WeakObject(ecs::obj object) :element(object) {

		}

		stn::Option<ecs::obj> get() const{
			if (element.exists()) {
				return element;

			}
			return stn::None;
		}
		bool alive() const {
			return element.exists();

		}
		explicit operator bool() const {
			return alive();
		}
		void set(ecs::obj object) {
			element = object;
		}

	private:
		ecs::obj element;

	};


}