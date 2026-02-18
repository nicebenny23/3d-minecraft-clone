#include "game_object.h"
#pragma once
namespace ecs {
	//we can use this until merge it 
	template<ComponentType... Types>
	struct Constrained {

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
				return object.get_component<T>();
			}
		}

		template<ComponentType T>
		const T& get_component() const {
			if constexpr (stn::OneOf<T, Types...>) {
			
				if (*last_checked_ticks != ticks) {
					validate();
				}
				return object.get_component<T>();
			}
			else {
				return object.get_component<T>();
			}
		}


		void validate() const {
			if (object.has_components<Types...>()) {
				stn::TupleSet<Types&...> as_refs = object.world().get_tuple_unchecked<Types...>(object.id());
				ticks = *last_checked_ticks;
				contained = stn::TupleSet((&as_refs.get<Types&>())...);
			}
			else {
				if (object.exists()) {
					stn::throw_logic_error("Constrained object Is missing components");
				}
				stn::throw_logic_error("Constrained object does not exist");
			}
		}
		Constrained(ecs::obj obj) :object(obj), ticks(), last_checked_ticks(ticks), contained(nullptr) {
			last_checked_ticks.observe(object.world().archetypes.archetype_entity_of(object.id()).ticks);
			validate();
		}
		template<stn::ReducerType T> requires stn::TupleSuperset<typename stn::reducer_held_type<T>::tuple_set_type, tuple_set_type>
		Constrained(T&& value) :
			ticks(value.reduction.ticks),
			object(value.reduction.object),
			last_checked_ticks(value.reduction.last_checked_ticks),
			contained(value.reduction.contained.reduce()) {

		}

		stn::SetReducer<Constrained> reduce(this auto&& self) {
			return stn::SetReducer<Constrained>(std::forward<decltype(self)>(self));
		}
		ecs::Ecs& world() {
			return object.world();
		}
		ecs::obj get() const {
			return object;
		}
		void destroy() {
			object.destroy();
		}
	private:

		ecs::obj object;
		mutable ecs::archetype_ticks ticks;
		using tuple_set_type = stn::TupleSet<Types*...>;
		stn::non_null < ecs::archetype_ticks> last_checked_ticks;
		mutable stn::TupleSet<Types*...> contained;
		template<ComponentType... others>
		friend struct Constrained;
	};






}