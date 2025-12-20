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
		stn::span<const component_id> view_cached() const {
			return cached_ids.span();
		}
		
		bool empty() const {
			return elems.empty();
		}

		archetype_id id() const {
			return Id;
		}
		std::uint32_t count() const {
			return std::uint32_t(elems.length());
		}

		bool contains_index(archetype_index index) const {
			return elems.contains_index(index.index);
		}
		
		entity& operator[](archetype_index arch_index) {
			return elems[arch_index.index];
		}
		const entity& operator[](archetype_index arch_index) const {
			return elems[arch_index.index];
		}

		bool passes(const archetype_predicate& pred) const {
			return pred.passes(id_set);
		}
		const entity& last() const {
			return elems.last();
		}
		entity& last() {
			return elems.last();
		}
		

		archetype_index last_index() const {
			return archetype_index(static_cast<uint32_t>(elems.last_index()));
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
		archetype_location add(entity& element) {
			elems.push(element);
			return archetype_location(id(), last_index());
		}

		void remove(archetype_index index) {
			if (!elems.contains_index(index.index)) {
				stn::throw_range_exception("cannot remove entity at index {} from archetype of size {}", index.index, count());
			}
			elems.swap_drop(index.index);
		}


		stn::Option<archetype_id> arch_connection_at(component_id index) const {
			if (moves.contains_index(index.id)) {
				return moves[index.id];
			}
			return stn::None;
		}


		using iterator = typename stn::array<entity>::iterator;

		iterator begin() {
			return elems.begin();
		}
		iterator end() {
			return elems.end();
		}

		component_ids id_set;
		stn::array<entity> elems;
		stn::array<stn::Option<archetype_id>> moves;
	private:
		stn::array<component_id> cached_ids;
		archetype_id Id;
	};



	// A list of Archetypes
	struct Archetypes {

		Archetypes(size_t size) :locations(size, archetype_location(archetype_id(0), archetype_index(0))) {
			//empty archetype
			add_archetype(component_ids(stn::None));
		};
		inline std::uint32_t total_archetypes() const {
			return static_cast<uint32_t>(archetypes.length());
		}
		const Archetype& empty_archetype() const {
			return archetypes[0];
		}
		Archetype& empty_archetype() {
			return archetypes[0];
		}
		Archetype& archetype_at(archetype_id Id) {
			return archetypes[Id.id];
		}
		const Archetype& archetype_at(archetype_id Id) const {
			return archetypes[Id.id];
		}
		entity& entity_at(archetype_location location) {
			return archetype_at(location.id)[location.index];
		}
		const entity& entity_at(archetype_location location) const {
			return archetype_at(location.id)[location.index];
		}
		archetype_location& location_of(entity ent) {
			return locations[ent.id()];
		}
		const archetype_location& location_of(entity ent) const {
			return locations[ent.id()];
		}
		const Archetype& archetype_of(entity ent) const {
			return archetype_at(location_of(ent).id);
		}
		Archetype& archetype_of(entity ent) {
			return archetype_at(location_of(ent).id);
		}
		bool contains(archetype_id id) const {
			return archetypes.contains_index(id.id);
		}
		bool contains(archetype_location location) const {
			if (!contains(location.id)) {
				return false;
			}
			return archetype_at(location.id).contains_index(location.index);
		}

		void expandArchetype() {
			for (auto& arch : archetypes) {
				arch.moves.expand(arch.moves.length() + 1);
			}
		}
		void add_to_empty(entity new_spawn) {
			locations[new_spawn.id()] = empty_archetype().add(new_spawn);
		}
		void add_archetype(const component_ids& Components) {
			for (Archetype& arch : archetypes) {
				if (arch.id_set == Components) {
					stn::throw_logic_error("Archetypes cannot contain 2 archetypes with the same bitset");
				}
			}
			archetypes.push(Archetype(Components, archetype_id(total_archetypes())));
			for (auto& arch : archetypes) {
				component_ids component_links = arch.id_set.flipped(Components);
				// If two archetypes differ by exactly one component, link their moves
				if (component_links.total_ids() == 1) {
					component_id other_index = component_links.into_list().first();
					archetypes.last().moves.reach(other_index.id) = arch.id();
					arch.moves.reach(other_index.id) = archetypes.last().id();

				}
			}
		}

		stn::array<archetype_id> archetypes_passing(const archetype_predicate& predicate) const {
			return
				archetypes
				.filter([&predicate](auto& archetype) {return archetype.passes(predicate); })
				.map([](const Archetype& archetype) {return archetype.id(); })
				.into<stn::array>();
		}

		inline void remove_from_archetypes(entity ent) {
			archetype_location entity_location = location_of(ent);
			Archetype& old_archetype = archetype_at(entity_location.id);
			location_of(old_archetype.last()) = entity_location;
			old_archetype.remove(entity_location.index);
		}

		archetype_location transfer_entity_to_flipped_index(entity ent, component_id index) {
			//same subroutine as remove_from_archetypes but does not seem to optimize correctly
			archetype_location& entity_location = location_of(ent);
			Archetype& old_archetype = archetype_at(entity_location.id);
			location_of(old_archetype.last()).index = entity_location.index;
			old_archetype.remove(entity_location.index);
			//get the archetype that is inverted at index
			stn::Option<archetype_id> new_id_opt = old_archetype.arch_connection_at(index);
			if (!new_id_opt) {
				//in this case we have not created an archetype connected to old_archetype at index yet
				component_ids new_arch = old_archetype.id_set.flipped(index);
				add_archetype(new_arch);
				//we cannot use old_archetype because a new one was added
				new_id_opt = archetype_at(entity_location.id).arch_connection_at(index);
				if (!new_id_opt) {
					stn::throw_logic_error("Failed to create a new archetype when flipping component id {} for entity {}", index.id, ent.id());
				}
			}
			archetype_id new_archetype_id = new_id_opt.unwrap();
			entity_location = archetype_at(new_archetype_id).add(ent);
			return entity_location;
		}

		using iterator = typename stn::array<Archetype>::iterator;
		iterator begin() {
			return archetypes.begin();
		}
		iterator end() {
			return archetypes.end();
		}

		using const_iterator = typename stn::array<Archetype>::const_iterator;
		const const_iterator begin() const {
			return archetypes.begin();
		}
		const const_iterator end() const {
			return archetypes.end();
		}

	private:
		stn::array<archetype_location> locations;
		stn::array<Archetype> archetypes;

	};
}