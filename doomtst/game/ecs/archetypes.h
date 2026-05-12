#pragma once
#include "archtype.h"
namespace ecs {
	using archetype_ticks = stn::ticks<archetype_location>;
	struct ArchetypeEntity {
		ArchetypeEntity() :location(archetype_id(0), archetype_index(0)) {
		}
		archetype_id id() const {
			return location.id;
		};

		archetype_location location;
		archetype_ticks ticks;
		void set(archetype_location new_location) {
			if (location.id != new_location.id) {
				ticks.tick();
			}
			location = new_location;
		}
		void set_index(archetype_index index) {
			location.index = index;
		}
	};

	// A list of Archetypes and their associated metadata
	struct Archetypes {

		inline std::uint32_t total_archetypes() const {
			return static_cast<uint32_t>(archetypes.length());
		}

		const Archetype& empty_archetype() const {
			return archetypes.first_unchecked();
		}
		Archetype& empty_archetype() {
			return archetypes.first_unchecked();
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
		ArchetypeEntity& reaching_archetype_entity_of(entity_id ent) {
			return locations.reach(ent.id);
		}
		ArchetypeEntity& archetype_entity_of(entity_id ent) {
			return locations[ent.id];
		}
		const ArchetypeEntity& archetype_entity_of(entity_id ent) const {
			return locations[ent.id];
		}
		ArchetypeEntity& archetype_entity_of_unchecked(entity_id ent) {
			return locations.unchecked_at(ent.id);
		}
		const ArchetypeEntity& archetype_entity_of_unchecked(entity_id ent) const {
			return locations.unchecked_at(ent.id);
		}

		Archetype& archetype_of(entity_id ent) {
			return archetype_at(archetype_entity_of(ent).id());
		}
		Archetype& archetype_of_unchecked(entity_id ent) {
			return archetype_at_unchecked(archetype_entity_of_unchecked(ent).id());
		}

		void spawn_at(entity_id new_spawn, archetype_id id) {
				reaching_archetype_entity_of(new_spawn).set(archetype_at_unchecked(id).add(new_spawn));
		}
		Archetypes(){
			//empty archetype
			insert_archetype(component_ids(stn::None));
		};
		void add_page(page_index index) {
			
		}
		void add_to_empty(entity_id new_spawn) {
			reaching_archetype_entity_of(new_spawn).set(empty_archetype().add(new_spawn));
		}

		archetype_id insert_archetype(const component_ids& components) {
			for (Archetype& arch : archetypes) {
				if (arch.id_set == components) {
					return arch.id();
				}
			}
			archetypes.push(Archetype(components, archetype_id(total_archetypes())));
			for (auto& arch : archetypes) {
				component_ids component_links = arch.id_set.flipped(components);
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
		//internal method that does not satify the ticks invariant;
		inline Archetype& remove_from_archetypes_storage_unchecked(archetype_location entity_location) {
			Archetype& old_archetype = archetype_at_unchecked(entity_location.id);
			archetype_entity_of_unchecked(old_archetype.last_unchecked()).set_index(entity_location.index);
			old_archetype.remove_unchecked(entity_location.index);
			return old_archetype;
		}

		inline void remove_from_archetypes_unchecked(entity_id ent) {
			ArchetypeEntity& entity_location = archetype_entity_of_unchecked(ent);
			entity_location.ticks.tick();
			remove_from_archetypes_storage_unchecked(entity_location.location);
		}

		void transfer_entity_to_flipped_index(entity_id ent, component_id index) {
			ArchetypeEntity& entity_location = archetype_entity_of(ent);
			//get the archetype that is inverted at index
			archetype_id new_id_opt =
				remove_from_archetypes_storage_unchecked(entity_location.location)
				.arch_connection_at(index)
				.unwrap_or_else([&]() {
				//in this case we have not created an archetype connected to old_archetype at index yet
				component_ids new_arch = archetype_at(entity_location.id()).id_set.flipped(index);
				return insert_archetype(new_arch);
			});
			entity_location.set(archetype_at_unchecked(new_id_opt).add(ent));
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
		stn::paged_array<ArchetypeEntity> locations;
		stn::array<Archetype> archetypes;

	};
}