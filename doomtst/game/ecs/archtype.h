#include "entities.h"
#include "../../util/bitset.h"
#include "component_id.h"
#include "../../util/counter.h"
namespace ecs {
	struct component_id_tag {};
	struct Archtype {
		
		Archtype(component_ids ids, archetype_id id) :id_set(ids), elems(), Id(id) {
			cached_ids = id_set.into_list();
			for (size_t ind = 0; ind < id_set.ids.bits; ind++) {
				moves.push(stn::None);
			}
		};
		const stn::array< archetype_id>& view_cached() const{
			return cached_ids;
		}
		archetype_id id() const {
			return Id;
		}
		std::uint32_t count() const {
			return elems.length();
		}
		entity& operator[](std::uint32_t arch_index) {
			return elems[arch_index];
		}
		const entity& operator[](std::uint32_t arch_index) const {
			return elems[arch_index];
		}

		entity& operator[](archetype_index arch_index) {
			return elems[arch_index.index];
		}
		const entity& operator[](archetype_index arch_index) const {
			return elems[arch_index.index];
		}

		bool passes(const archetype_predicate& pred) const{
			return pred.passes(id_set);
		}
		const entity& last() const {
			return elems.last();
		}
		entity& last() {
			return elems.last();
		}
		
		archetype_index last_index() const{
			return archetype_index(elems.last_index());
		}
		
		bool has_component(component_id id) const {
			return id_set[id];
		}

		bool has_components(const component_ids& set) const {
			return id_set.has(set);
		}
		bool has_none(const component_ids& set) const
		{
			return id_set.has_none(set);
		}
		
		//returns the new archetype location
		archetype_location add(entity& entityect) {
			elems.push(entityect);
			return archetype_location(id(), last_index());
		}

		
		void remove(archetype_index index) {

			if (!elems.contains_index(index.index)) {
		
				throw stn::make_range_exception("cannot remove entity at index {} from archtype of size {}", index.index, count());
			}
			elems.swap_drop(index.index);
		}

		
		stn::Option<archetype_id> arch_connection_at(component_id index) const {
			if (moves.contains_index(index.id))
			{
				return moves[index.id];
			}
			return stn::None;
		}
		
		
		using iterator = typename stn::array<entity>::iterator;

		iterator begin() { return elems.begin(); }
		iterator end() { return elems.end(); }

		component_ids id_set;
		stn::array<entity> elems;
		stn::array<stn::Option<archetype_id>> moves;
	private:
		stn::array<component_id> cached_ids;
		archetype_id Id;
	};



	// A list of Archtypes
	struct Archtypes {
	
		Archtypes():locations() {
			//empty archtype
			add_archetype(component_ids(stn::None));
		};
		inline size_t total_archetypes() const {
			return archtypes.length();
		}
		const Archtype& empty_archtype() const {
			return archtypes[0];
		}
		Archtype& empty_archtype() {
			return archtypes[0];
		}
		Archtype& archetype_at(archetype_id Id) {
			return archtypes[Id.id];
		}
		const Archtype& archetype_at(archetype_id Id) const {
			return archtypes[Id.id];
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
		const archetype_location& location_of(entity ent) const{
			return locations[ent.id()];
		}
		const Archtype& archetype_of(entity ent) const {
			return archetype_at(location_of(ent).id);
		}
		Archtype& archetype_of(entity ent) {
			return archetype_at(location_of(ent).id);
		}


		void expandArchtype() {

			for (auto& arch : archtypes)
			{
				arch.moves.expand(arch.moves.length() + 1);
			}

		}
		void add_to_empty(entity new_spawn) {
			locations[new_spawn.id()] = empty_archtype().add(new_spawn);
		}
		void add_archetype(const component_ids& Components) {
			for (Archtype& arch : archtypes) {
				if (arch.id_set == Components) {
					throw std::logic_error("Archetypes cannot contain 2 archtypes with the same bitset");
				}
			}
			archtypes.push(Archtype(Components, archetype_id(total_archetypes())));
			for (auto& arch : archtypes)
			{
				component_ids component_links = arch.id_set.flipped(Components);
				// If two archetypes differ by exactly one component, link their moves
				if (component_links.total_ids() == 1)
				{
					component_id other_index = component_links.into_list().first();
					archtypes.last().moves.reach(other_index.id) = arch.id();
					arch.moves.reach(other_index.id) = archtypes.last().id();

				}
			}
		}
		
		stn::array<archetype_id> archetypes_passing(const archetype_predicate& predicate) const{
			stn::array<archetype_id> passes;
			for (auto& archetype :archtypes)
			{
				if (archetype.passes(predicate))
				{
					passes.push(archetype.id());
				}
			}
			return passes;
		}

		inline void remove_from_archetypes(entity ent) {
			archetype_location entity_location = location_of(ent);
			Archtype& old_archetype = archetype_at(entity_location.id);
			location_of(old_archetype.last())= entity_location;
			old_archetype.remove(entity_location.index);
		}

		archetype_location transfer_entity_to_flipped_index(entity ent, component_id index) {
			//same subroutine as above but does not seem to optimize correctly
			archetype_location& entity_location = location_of(ent);
			Archtype& old_archetype = archetype_at(entity_location.id);
			location_of(old_archetype.last()).index = entity_location.index;
			old_archetype.remove(entity_location.index);
			//get the archetype that is inverted at index
			stn::Option<archetype_id> new_id_opt = old_archetype.arch_connection_at(index);
			if (!new_id_opt)
			{
				//in this case we have not created an archetype connected to old_archetype at index yet
				component_ids new_arch = old_archetype.id_set;
				new_arch.flipped(index);
				add_archetype(new_arch);
				new_id_opt = old_archetype.arch_connection_at(index);
				if (!new_id_opt)
				{
					throw stn::make_logic_exception("Failed to create a new archetype when flipping component id {} for entity {}",index.id, ent.id());
				}
			}

			archetype_id new_archetype_id = new_id_opt.unwrap();
			entity_location= archetype_at(new_archetype_id).add(ent);
			return entity_location;
		}

		using iterator = typename stn::array<Archtype>::iterator;
		iterator begin() { return archtypes.begin(); }
		iterator end() { return archtypes.end(); }

	private:
		stn::array<archetype_location> locations;
		stn::array<Archtype> archtypes;
		
	};
}