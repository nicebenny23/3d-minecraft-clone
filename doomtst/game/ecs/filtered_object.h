#include "game_object.h"
#include "unique_object.h"
#pragma once
namespace ecs {
	//we can use this until merge it 
	template<ObjectLike Object, ComponentType... Types>
	struct ConstrainedObject {
		template<ComponentType T> requires stn::OneOf<T, Types...>
		T& get() {
			if (*last_checked_ticks != ticks) {
				validate();
			}
			return *contained.get<T*>();
		}
		template<ComponentType T> requires stn::OneOf<T, Types...>
		const T& get() const {
			if (*last_checked_ticks != ticks) {
				validate();
			}
			return *contained.get<T*>();
		}
		template<ComponentType T>
		T& get_component() {
			if constexpr (stn::OneOf<T, Types...>) {
				//adding this line should lead to the bp being hit or do nothing

				if (*last_checked_ticks != ticks) {
					validate();
				}
				return *contained.get<T*>();
			}
			else {
				return object().get_component<T>();
			}
		}

		template<ComponentType T>
		const T& get_component() const {
			if constexpr (stn::OneOf<T, Types...>) {

				if (*last_checked_ticks != ticks) {
					validate();
				}
				return *contained.get<T*>();
			}
			else {
				return object().get_component<T>();
			}
		}


		void validate() const {
			stn::TupleSet<Types&...> as_refs = world().get_components<Types...>(object().inner());
			ticks = *last_checked_ticks;
			contained = stn::TupleSet((&as_refs.get<Types&>())...);
		}
		template<typename Arg> requires std::constructible_from<Object, Arg&&>
		ConstrainedObject(Arg&& obj) :entity(std::forward<Arg>(obj)), ticks(), last_checked_ticks(ticks), contained(nullptr) {
			last_checked_ticks.observe(world().archetypes.archetype_entity_of(object().id()).ticks);
			validate();
		}
		
		
		template<stn::ReducerType T> requires stn::TupleSuperset<typename stn::reducer_held_type<T>::tuple_set_type, tuple_set_type>
		ConstrainedObject(T&& value) :
			ticks(value.reduction.ticks),
			entity(std::move(value.reduction.entity)),
			last_checked_ticks(value.reduction.last_checked_ticks),
			contained(value.reduction.contained.reduce()) {

		}

		stn::SetReducer<ConstrainedObject> reduce(this auto&& self) {
			return stn::SetReducer<ConstrainedObject>(std::forward<decltype(self)>(self));
		}
		ecs::Ecs& world() const {
			return as_object(entity).world();
		}
		ecs::obj object() const {
			return as_object(entity);
		}
		void destroy() {
			entity.destroy();
		}
	private:

		Object entity;
		mutable ecs::archetype_ticks ticks;
		using tuple_set_type = stn::TupleSet<Types*...>;
		stn::non_null < ecs::archetype_ticks> last_checked_ticks;
		mutable stn::TupleSet<Types*...> contained;
		template<ObjectLike U, ComponentType... others>
		friend struct ConstrainedObject;
	};

	template<ComponentType... Types>
	using Constrained = ConstrainedObject<ecs::obj, Types...>;
	template<ComponentType... Types>
	using ConstrainedHandle = ConstrainedObject<ecs::object_handle, Types...>;




}