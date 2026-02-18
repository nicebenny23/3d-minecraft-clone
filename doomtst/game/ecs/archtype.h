#pragma once
#include "entities.h"
#include "../../util/bitset.h"
#include "component_id.h"
#include "../../util/counter.h"
namespace ecs {
	struct Archetype {

		Archetype(component_ids comp_ids, archetype_id id) :id_set(comp_ids), elems(), Id(id) {
			cached_ids = id_set.into_list();
			moves.expand(id_set.id_bitset.bits);
		};

		archetype_id id() const {
			return Id;
		}
		bool contains_index(archetype_index index) const {
			return elems.contains_index(index.index);
		}

		bool empty() const {
			return elems.empty();
		}

		stn::span<const component_id> view_cached() const {
			return cached_ids.span();
		}

		std::uint32_t count() const {
			return std::uint32_t(elems.length());
		}



		entity_id& operator[](archetype_index arch_index) {
			return elems[arch_index.index];
		}
		const entity_id& operator[](archetype_index arch_index) const {
			return elems[arch_index.index];
		}

		bool passes(const archetype_predicate& pred) const {
			return pred.passes(id_set);
		}

		archetype_index last_index() const {
			return archetype_index(static_cast<uint32_t>(elems.last_index()));
		}

		archetype_index last_index_unchecked() const {
			return archetype_index(static_cast<uint32_t>(elems.last_index_unchecked()));
		}

		const entity_id& last() const {
			return elems.last();
		}
		entity_id& last() {
			return elems.last();
		}
		bool has_component(component_id id) const {
			return id_set[id];
		}

		bool has_components(const component_ids& set) const {
			return id_set.has(set);
		}
		bool has_none(const component_ids& set) const {
			return id_set.has_none(set);
		}

		//returns the new archetype location
		archetype_location add(const entity_id& element) {
			return archetype_location(id(), archetype_index(static_cast<std::uint32_t>(elems.push_index(element))));
		}

		void remove(archetype_index index) {
			if (!elems.contains_index(index.index)) {
				stn::throw_range_exception("cannot remove entity_id at index {} from archetype of size {}", index.index, count());
			}
			elems.swap_drop_unchecked(index.index);
		}
		void remove_unchecked(archetype_index index) {
			elems.swap_drop_unchecked(index.index);
		}


		stn::Option<archetype_id> arch_connection_at(component_id index) const {
				return moves.get_flat(index.id);
		}


		using iterator = typename stn::array<entity_id>::iterator;

		iterator begin() {
			return elems.begin();
		}
		iterator end() {
			return elems.end();
		}

		component_ids id_set;
		stn::array<entity_id> elems;
		stn::array<stn::Option<archetype_id>> moves;
	private:
		stn::array<component_id> cached_ids;
		archetype_id Id;
	};
}