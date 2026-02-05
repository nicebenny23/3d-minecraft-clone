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



	// A list of Archetypes
	struct Archetypes {


		inline std::uint32_t total_archetypes() const {
			return static_cast<uint32_t>(archetypes.length());
		}
		const Archetype& empty_archetype() const {
			return archetypes[0];
		}
		Archetype& empty_archetype() {
			return archetypes[0];
		}
		const Archetype& archetype_at_unchecked(archetype_id id) const {
			return archetypes.unchecked_at(id.id);
		}
		Archetype& archetype_at_unchecked(archetype_id id) {
			return archetypes.unchecked_at(id.id);
		}

		Archetype& archetype_at(archetype_id id) {
			return archetypes[id.id];
		}

		const Archetype& archetype_at(archetype_id id) const {
			return archetypes[id.id];
		}
		bool contains(archetype_id id) const {
			return archetypes.contains_index(id.id);
		}
		bool contains(archetype_location location) const {
			if (!contains(location.id)) {
				return false;
			}
			return archetypes.unchecked_at(location.id.id).contains_index(location.index);
		}

		entity_id& entity_at(archetype_location location) {
			return archetype_at(location.id)[location.index];
		}
		const entity_id& entity_at(archetype_location location) const {
			return archetype_at(location.id)[location.index];
		}
		archetype_location& location_of(entity_id ent) {
			return locations[ent.id];
		}
		const archetype_location& location_of(entity_id ent) const {
			return locations[ent.id];
		}
		const Archetype& archetype_of(entity_id ent) const {
			return archetype_at(location_of(ent).id);
		}
		Archetype& archetype_of(entity_id ent) {
			return archetype_at(location_of(ent).id);
		}
		void spawn_at(entity_id new_spawn, component_id component_id) {
			archetype_id spawn_at = empty_archetype().arch_connection_at(component_id).unwrap_or_else([&]() {
				//in this case we have not created an archetype connected to old_archetype at index yet
				return add_archetype(empty_archetype().id_set.flipped(component_id));
				});
			locations[new_spawn.id] = archetype_at(spawn_at).add(new_spawn);
		}
		Archetypes(size_t size) :locations(size, archetype_location(archetype_id(0), archetype_index(0))) {
			//empty archetype
			add_archetype(component_ids(stn::None));
		};
		void add_to_empty(entity_id new_spawn) {
			locations[new_spawn.id] = empty_archetype().add(new_spawn);
		}

		archetype_id add_archetype(const component_ids& Components) {
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
			return archetypes.last().id();
		}

		stn::array<archetype_id> archetypes_passing(const archetype_predicate& predicate) const {
			return
				archetypes
				.filter([&predicate](auto& archetype) {return archetype.passes(predicate); })
				.map([](const Archetype& archetype) {return archetype.id(); })
				.into<stn::array>();
		}
		//returns the id of the old archetype
		inline void remove_from_archetypes_storage_unchecked(archetype_location entity_location) {
			Archetype& old_archetype = archetype_at(entity_location.id);
			location_of(old_archetype.last()) = entity_location;
			old_archetype.remove_unchecked(entity_location.index);

		}
		inline void remove_from_archetypes_unchecked(entity_id ent) {
			remove_from_archetypes_storage_unchecked(location_of(ent));
		}


		archetype_location transfer_entity_to_flipped_index(entity_id ent, component_id index) {
			archetype_location& entity_location = location_of(ent);
			//get the archetype that is inverted at index
			remove_from_archetypes_storage_unchecked(entity_location);
			archetype_id new_id_opt =
				archetype_at_unchecked(entity_location.id)
				.arch_connection_at(index)
				.unwrap_or_else([&]() {
				//in this case we have not created an archetype connected to old_archetype at index yet
				component_ids new_arch = archetype_at_unchecked(entity_location.id)
					.id_set.flipped(index);
				return add_archetype(new_arch);
					});
			entity_location = archetype_at(new_id_opt).add(ent);
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