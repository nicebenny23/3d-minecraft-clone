#pragma once
#include "archtype.h"
#include "ecs.h"
#include "component_id.h"
#include "entities.h"
#include "entity.h"
#include "../../util/dynamicarray.h"
#include "../../util/exception.h"
#include "../../util/Span.h"
#include "game_object.h"
namespace ecs {
	// Views iterate over entities containing the specified Components.
	
	
	template<typename T>
	concept QueryFilter = requires(T t, const Archetype & archetype) {
		{
			t.filter(archetype)
		} -> std::convertible_to<bool>;
	};
	template<typename T>
	concept QueryMap = requires(const T& t, entity_id ent) {
		{
			t.map(ent)
		};
	};
	template<typename T>
	struct query_map_t:std::bool_constant<QueryMap<T>>{

	};

	template<typename T>
	struct query_filter_t :std::bool_constant<QueryFilter<T>> {

	};
	struct Owner {

		ecs::obj map(entity_id ent) const {
			return ecs::obj(ecs::entity(ent,world.entities.at(ent).gen_count),world);
		}
		Owner(ecs::Ecs& world) :world(world) {

		}
		ecs::Ecs& world;
	};
	
	template<ComponentType T>
	struct With {
		T& map(entity_id ent) const {
			return world.get_component_unchecked<T>(ent);
		}
		bool filter(const Archetype& archetype) const {
			return archetype.has_component(id_for);
		}
		With(ecs::Ecs& world) :world(world), id_for(world.insert_component_id<T>()) {

		}
		ecs::Ecs& world;
		component_id id_for;
	};
	
	template<typename T>
	struct query_data_t {
		using type = T;
	};
	template<ComponentType T>
	struct query_data_t<T> {
		using type =  ecs::With<T>;
	};

	
	template<typename T>
	concept QueryElement = QueryMap<T> || QueryFilter<T> && std::constructible_from<T, Ecs&>;

	//special casing for components so that we can write View<Position,Velocity> instead of View<With<Position>, With<Velocity>>
	template<typename T>
	using query_data = typename query_data_t<T>::type;
	template<typename T>
	concept HasAssociatedQuery = QueryElement<query_data<T>>;

	template<ComponentType T>
	struct Mabye {
		stn::Option<T&> map(entity_id ent) const {
			return world.get_component_opt<T>(ent);
		}
		Mabye(ecs::Ecs& world) :world(world), id_for(world.insert_component_id<T>()) {

		}
		ecs::Ecs& world;
		component_id id_for;
	};
	template<ComponentType T>
	struct Has {
		Has(ecs::Ecs& world) :id_for(world.insert_component_id<T>()) {
		}
		bool filter(const Archetype& archetype) const {
			return archetype.has_component(id_for);
		}
		component_id id_for;
	};

	template<HasAssociatedQuery... Components>
	struct View {
		Ecs& world() {
			return ecs;
		}
		stn::span<archetype_id> view_archetypes() {
			return archetypes.span();
		}
		using set_type = stn::TupleSet<query_data<Components>...>;
		using filter_set = stn::subset_passing<set_type, query_filter_t>;
		using map_set = stn::subset_passing<set_type, query_map_t>;
		struct Iterator {
			View& owner;
			size_t archetype_list_index;
			archetype_index arch_index;
			Iterator(View& vw, size_t list_index, archetype_index arch_ind)
				:owner(vw), archetype_list_index(list_index), arch_index(arch_ind) {
			}
			
			bool find_next_entry() {

				//finds the next valid/final entry, returning whether it was a valid entry
				while (owner.archetypes.contains_index(archetype_list_index)) {
					Archetype& arch = owner.ecs.archetypes.archetype_at(owner.archetypes[archetype_list_index]);
					if (arch.contains_index(arch_index)) {
						return true;
					}
					//reset to next archetype
					arch_index = archetype_index(0);
					archetype_list_index++;
				}
				return false;
			}

			Iterator& operator++() {
				arch_index++;
				if (!find_next_entry()) {
					//because we are at end
					arch_index = archetype_index(0);
				}
				return *this;
			}

			bool operator==(const Iterator& other) const {
				return archetype_list_index == other.archetype_list_index && arch_index == other.arch_index;
			}

			bool operator!=(const Iterator& other) const {
				return !(*this == other);
			}

			auto operator*() {
				if (!owner.archetypes.contains_index(archetype_list_index)) {
					stn::throw_logic_error("Cannot dereference iterator: reached end of View");
				}
				entity_id ent = owner.ecs.archetypes.archetype_at(owner.archetypes.unchecked_at(archetype_list_index))[arch_index];
				auto map = [ent] <typename T>(T& mapper) ->decltype(auto){
					return mapper.map(ent);
				};
				return  stn::map_tuple(owner.getters, map);
			}

		};

		Iterator end() {
			//[total_archetypes,0] is the sentinel state
			return Iterator(*this, total_archetypes(), archetype_index(0));
		}

		Iterator begin() {
			if (archetypes.empty()) {
				return end();
			}
			Iterator it(*this, 0, archetype_index(0));
			it.find_next_entry();
			return it;
		}

		View(Ecs& world) :ecs(world), archetypes(), getters(world) {
			filter_set filters(world);
			for (Archetype& archetype: ecs.archetypes) {
				auto filter_function = [&archetype] <typename T>(T & filter) {
					return filter.filter(archetype);
				};
				if (stn::all_tuple(filters, filter_function))
					{
						archetypes.push(archetype.id());
					}
				}
		}

		//returns the archetype at index in archetypes
		Archetype& archetype_at(size_t index) {
			return ecs.archetypes.archetype_at(archetypes[index]);
		}

		//returns the total number of archetypes in this view
		size_t total_archetypes() const {
			return archetypes.length();
		}

	private:
		Ecs& ecs;
		friend struct Iterator;
		map_set getters;
		stn::array<archetype_id> archetypes;
	};
}