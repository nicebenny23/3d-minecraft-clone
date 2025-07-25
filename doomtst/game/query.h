#pragma once
#include "gameobject.h"
#include "../util/filter_argument.h"
#include "../util/Not.h"
#include "../util/typelist.h"
namespace query {
	enum class query_argument_type {
		Has,
		Option,
		Not,
		

	};
	template <typename T>
	struct QueryArgument {
		using type = T;
		static constexpr query_argument_type arg_type= query_argument_type::Has;
	};
	template <typename T>
	struct QueryArgument<Not<T>> {
		using type = T;
		static constexpr query_argument_type arg_type = query_argument_type::Not;
	};
	template <typename T>
	struct QueryArgument<T*> {
		using type = T;
		static constexpr query_argument_type arg_type = query_argument_type::Option;
	};

	template<typename T>
	struct neededPredicate {
		static constexpr bool value = QueryArgument<T>::arg_type == query_argument_type::Has;
	}; 
	
	template<typename T>
	struct resultPredicate {
		static constexpr bool value = QueryArgument<T>::arg_type != query_argument_type::Not;
	};
	template<typename T>
	struct excludedPredicate {
		static constexpr bool value = QueryArgument<T>::arg_type == query_argument_type::Not;
	};

	template<typename... Components>
	struct View {
		stn::array<gameobject::Archtype*> archtypes;
		gameobject::Ecs* ecs;
		
		array<comp::Id> positions;
		struct Iterator {
			View& owner;
			size_t entity_index;
			size_t arch_index;

			Iterator(View& vw, size_t ent_index = 0,size_t ArchIndex=0)
				:owner(vw), arch_index(ArchIndex), entity_index(ent_index) {
			}

			Iterator& operator++() {
				entity_index++;

				// loop until we find a valid entity in a valid archetype
				while (arch_index < owner.archtypes.length) {
					auto& arch = owner.archtypes[arch_index];
					if (entity_index < arch->elems.length) {
						break;
					}
					arch_index++;
					entity_index = 0;
				}

				return *this;
			}

			bool operator==(const Iterator& other) const {
				return arch_index == other.arch_index && entity_index == other.entity_index;
			}
			bool operator!=(const Iterator& other) const {
				return !(*this == other);
			}
			auto operator*() {
				if (
					owner.archtypes[arch_index]->elems.length == 0) {
					throw std::out_of_range("Invalid dereference: View is empty or index out of range.");
				}
				auto arch = owner.archtypes[arch_index];
				return owner.ecs->get_tuple<Components...>(arch->elems[entity_index],owner.positions);
			}
		};

		Iterator begin() {
			Iterator it(*this, 0, 0);

			// Skip empty archetypes at the beginning
			while (it.arch_index < archtypes.length &&
				archtypes[it.arch_index]->elems.length == 0) {
				++it.arch_index;
			}

			return it;
		}
		Iterator end() {
			return Iterator(*this, 0, archtypes.length);
		}
		View(gameobject::Ecs* world):ecs(world), archtypes(),positions(){
			if (!ecs->component_indexer.contains_all<Components...>())
			{
				return;
			}
			positions = ecs->component_indexer.get_type_ids<Components...>();

			bitset::bitset bitlist;
			bitlist.expand(ecs->component_indexer.size());
			for (auto id : positions) {
				bitlist.set(id.id);
			}
			for (auto& arch : ecs->arch) {
				if (arch->has_components(bitlist)) {
					archtypes.push(arch);
				}
			}
		
		}
		~View() {

			archtypes.destroy();
		}
	};

	template<typename... Components>
	struct ViewBuilder {
		using needed = type_pipeline::Filter_t<neededPredicate, Components...>::type;
		using excluded = type_pipeline::Filter_t<excludedPredicate, Components...>::type;
		using result = type_pipeline::Filter_t<resultPredicate, Components...>::type;
		View<result> Build(gameobject::Ecs* ecs) {
			View<result> output_arch;
			output_arch.ecs = ecs;
			bitset::bitset exclude;
			bitset::bitset require;
			stn::array<comp::Id> output;

			output = ecs->component_indexer.get_type_ids<result...>();
			require = ecs->bitset_of<needed...>();
			exclude = ecs->bitset_of<excluded...>();
			for (auto arch : ecs->arch.archtypes)
			{
				if (arch->has_components(require) && !arch->has_any(exclude))
				{
					output_arch.archtypes.push(arch);

				}

			}
			output_arch.positions = output;
			return output_arch;
		}
		  
	};

	struct ComponentIterator {
		gameobject::obj owner;
		gameobject::Ecs* manager;
		const array<size_t>& dense_bits;
		size_t ind = 0;

		ComponentIterator(gameobject::obj obj, gameobject::Ecs* man, const array<size_t>& bits, size_t index)
			: owner(obj), manager(man), dense_bits(bits), ind(index) {
		}

		gameobject::component* operator*() {
			size_t comp_id = dense_bits[ind];
			auto& store = manager->comp_storage[comp_id].store;
			gameobject::component* value = store[owner.GenId.id];
			if (value == nullptr) {
				// FAIL FAST with a clear error
				throw std::logic_error(
					"ECS invariant violated: archetype lists comp_id " +
					std::to_string(comp_id) +
					" for entity " + std::to_string(owner.GenId.id) +
					", but store does not contain that key.");
			}
			return value;
		}
		ComponentIterator& operator++() {
			++ind;
			return *this;
		}

		bool operator!=(const ComponentIterator& other) const {
			return ind != other.ind;
		}
	};
	//will not care if component list has been changed during the iteration
	struct ComponentView {
		gameobject::obj owner;
		array<size_t> dense_bits;
		ComponentView(gameobject::obj o) : owner(o) {
			dense_bits = owner.meta().arch->bit_list.indices();
		}

		ComponentIterator begin() {
			return ComponentIterator(owner, owner.OC, dense_bits, 0);
		}
		~ComponentView() {
			dense_bits.destroy();
		}
		ComponentIterator end() {
			return ComponentIterator(owner, owner.OC, dense_bits, dense_bits.length);
		}
	};
}