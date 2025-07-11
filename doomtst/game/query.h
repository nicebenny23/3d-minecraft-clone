#pragma once
#include "gameobject.h"


namespace query {


	template<typename... Components>
	struct ArchFilter {
	};


	template<typename... Components>
	struct View {
		Cont::array<gameobject::Archtype*> archtypes;
		gameobject::Ecs* ecs;
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
				size_t entity_id = arch->elems[entity_index].Id.id;
				return std::tuple<Components*...>{owner.ecs->getcomponentptr<Components>(arch->elems[entity_index])... };
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
		View(gameobject::Ecs* world):ecs(world) {
			auto positions = ecs->comp_map.get_type_ids<Components...>();
			
			bitset::bitset bitlist;
			bitlist.expand(ecs->comp_map.size());
			for (auto id : positions) {
				bitlist.set(id.value);
			}
			Cont::array<gameobject::Archtype*> result;
			for (auto& arch : ecs->arch) {
				if (arch->has_components(bitlist)) {
					result.push(arch);
				}
			}
			archtypes = result;
		
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