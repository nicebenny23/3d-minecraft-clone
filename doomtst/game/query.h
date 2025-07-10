#pragma once
#include "gameobject.h"


namespace query {


	template<typename... Components>
	struct ArchFilter {
		bitset::bitset bitlist;
		gameobject::Ecs* manager;
		ArchFilter(gameobject::Ecs* ecs) : manager(ecs) {
			auto positions = manager->comp_map.get_type_ids<Components...>();
			bitlist.resize(manager->managers.length);
			for (auto id : positions) {
				bitlist.set(id.value);
			}
		}

	};


	template<typename... Components>
	struct View {
		Cont::array<gameobject::Archtype*> archetypes;
		gameobject::Ecs* ecs;
		struct Iterator {
			size_t entity_index;
			size_t arch_index;
			Iterator(array<gameobject::Archtype>& arch, size_t ent_index = 0)
				: arch_index(arch), entity_index(ent_index) {
			}

			Iterator& operator++() {
				entity_index++;
				auto& arch = ecs->arch[arch_index.arch_index];
				if (entity_index >= arch.elems.length) {
					entity_index = 0;
					arch_index++;
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
				auto& arch = arch[arch_index];
				size_t entity_id = arch.elems[entity_index].Id.id;
				return std::tuple<Components*...>{ecs->getcomponentptr<Components>(arch.elems[entity_index])... };
			}
		};

		Iterator begin();
		Iterator end();
		View(ArchFilter<Components...> filter, gameobject::Ecs* ecs) {
			Cont::array<gameobject::Archtype*> result;
			for (auto& arch : ecs->arch) {
				if (arch.has_components(filter.bitlist)) {
					result.push(&arch);
				}
			}
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
			auto& store = manager->managers[comp_id].store;
			gameobject::component* value = store[owner.Id.id];
			if (value == nullptr) {
				// FAIL FAST with a clear error
				throw std::logic_error(
					"ECS invariant violated: archetype lists comp_id " +
					std::to_string(comp_id) +
					" for entity " + std::to_string(owner.Id.id) +
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

		ComponentView(gameobject::obj o) : owner(o) {}

		ComponentIterator begin() {
			auto& bits = owner.meta().arch->dense_bits;
			return ComponentIterator(owner, owner.OC, bits, 0);
		}

		ComponentIterator end() {
			auto* meta = owner.meta().arch;
			auto& bits = owner.meta().arch->dense_bits;
			return ComponentIterator(owner, owner.OC, bits, bits.length);
		}
	};
}