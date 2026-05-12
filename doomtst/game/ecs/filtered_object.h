#include "game_object.h"
#include "unique_object.h"
#include "query.h"
#pragma once
namespace ecs {
	//we may want too merge this with the query filter
	template<ObjectLike Object, ComponentType... Types>
	struct ConstrainedObject {

		template<ComponentType T> requires stn::OneOf<T, Types...>
		auto get(this auto&& self) -> stn::apply_const_like_t<decltype(self), T>& {
			self.validate();
			return *self.contained.get<T*>();
		}
		template<ComponentType T> requires stn::OneOf<T, Types...>
		auto get_unchecked(this auto&& self) -> stn::apply_const_like_t<decltype(self), T>& {
			return *self.contained.get<T*>();
		}
		template<ComponentType T>
		stn::Option<T&> get_component_opt() {
			return object().get_component_opt<T>();
		}
		template<ComponentType T>
		stn::Option<const T&> get_component_opt() const {
			return object().get_component_opt<T>();
		}
		template<ComponentType T>
		bool has_component() const {
			return object().has_component<T>();
		}
	
		template<ComponentType T>
		auto get_component(this auto&& self) -> stn::apply_const_like_t<decltype(self), T>& {
			if constexpr (stn::OneOf<T, Types...>) {
				self.validate();
				return *self.contained.template get<T*>();
			}
			else {
				return self.object().template get_component<T>();
			}
		}

		using tuple_set_type = stn::TupleSet<Types...>;
		using tuple_set_ptr_type = stn::TupleSet<Types*...>;
		//needed for weak
		bool is_valid() const {
			return world().contains(object().inner())&&object().has_components<Types...>();
		}
		void validate() const {
			if (*last_checked_ticks != ticks) {
				stn::TupleSet<Types&...> as_refs = world().get_components<Types...>(object().inner());
				ticks = *last_checked_ticks;
				contained = stn::TupleSet((&as_refs.get<Types&>())...);
			}
		}

		template<typename Arg> requires std::constructible_from<Object, Arg&&>
		ConstrainedObject(Arg&& obj) :entity(std::forward<Arg>(obj)), ticks(), last_checked_ticks(ticks), contained(nullptr) {
			last_checked_ticks.observe(world().archetypes.reaching_archetype_entity_of(object().id()).ticks);

			//we can just validate because if we have a Constrained object, its archtype ticks cannot be zero as that would mean they have not been initilized
			validate();
		}

		template<stn::ReducerType T> requires stn::TupleSuperset<typename stn::reducer_held_type<T>::tuple_set_ptr_type, tuple_set_ptr_type>
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
		ecs::entity_id id() const {
			return entity.id();
		}
		void destroy() {
			entity.destroy();
		}
		template<typename Arg> requires std::constructible_from<Object, Arg&&>
		static ConstrainedObject make_unchecked(Arg&& obj, stn::TupleSet<Types&...> components) {
			ecs::Ecs& world = obj.world();
			stn::non_null < ecs::archetype_ticks> last_checked_ticks(world.archetypes.archetype_entity_of(obj.id()).ticks);
			return ConstrainedObject(last_checked_ticks, obj, stn::TupleSet((&components.get<Types&>())...));
		}
		bool operator!=(const ConstrainedObject& other) const {
			return !(other == *this);
		}

		bool operator==(const ConstrainedObject& other) const {
			validate();
			other.validate();
			return other.entity == entity;
		}
		using ObjectType= ConstrainedObject<ecs::obj, Types...>;
		template<ObjectLike T>
		explicit ConstrainedObject(const ConstrainedObject<T, Types...>& other)
			:entity(other.object()), ticks(other.ticks)
		, last_checked_ticks(other.last_checked_ticks), contained(other.contained)
		{

		}
	
	private:


		ConstrainedObject(stn::non_null < ecs::archetype_ticks> last_checked, Object object, tuple_set_ptr_type components)
			:ticks(*last_checked), last_checked_ticks(last_checked), entity(object), contained(components) {

		}
		Object entity;
		mutable ecs::archetype_ticks ticks;
		stn::non_null < ecs::archetype_ticks> last_checked_ticks;
		mutable tuple_set_ptr_type contained;
		template<ObjectLike U, ComponentType... others>
		friend struct ConstrainedObject;
	};

	template<ComponentType... Types>
	using Constrained = ConstrainedObject<ecs::obj, Types...>;
	template<ComponentType... Types>
	using ConstrainedHandle = ConstrainedObject<ecs::object_handle, Types...>;
	
	template<typename T,typename ... Types>
	concept ConstrainedBy = (ComponentType<Types>&&...)&&requires{
		typename T::tuple_set_type;
	}&&stn::TupleSuperset<typename T::tuple_set_type,stn::TupleSet<Types...>>;


	template<ComponentType ...Types>
	struct ConstrainedGetter {
		ConstrainedGetter(ecs::Ecs& ecs) :world(ecs) {

		}
		bool filter(const Archetype& archetype) const {
			return archetype.has_components(world.components.insert_ids<Types...>());
		}
		
		Ecs& world;
		using return_type = Constrained<Types...>;
		return_type map(ecs::entity_id id) const {
			return return_type(world.object_from_entity(world.entities.get_entity_unchecked(id)));
		}
	};

	template<ComponentType ...Args> requires (sizeof...(Args)!=0)
	struct query_data_t<Constrained<Args...>> {
		using type = ConstrainedGetter<Args...>;
	};
}