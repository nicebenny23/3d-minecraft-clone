#include "entities.h"

#pragma once
namespace ecs {
	struct component_id_tag {};
	using component_id=Ids::typed_id<component_id_tag> ;
	struct component_indexer {

	};
	struct archetype_predicate;

	struct component_ids {

		component_ids(stn::NoneType) :ids() {};

		component_ids(stn::bitset id_list) :ids(id_list) {}
		stn::bitset ids;
		bool has(component_id id) const{
			return ids.contains_index(id.id) && ids[id.id];
		}
		bool has(const component_ids& other) const {
			return (ids&other.ids)== ids;
		}
		bool has_none(const component_ids& other) const {
			return (ids & other.ids)==stn::bitset();
		}
		

		stn::array<component_id> into_list() const{
			return ids.indices<component_id>();
		}

		bool operator[](component_id id) const {
			return has(id);
		}

		//need to make const
		size_t total_ids() const{
			return ids.popcount();
		}
		component_ids without(const component_ids& other) const {
			stn::bitset other_bits = other.ids;
			other_bits.expand(ids.bits);

			return ids & ~other_bits;
		}
		component_ids without(component_id id) const {
			component_ids new_id_set = component_ids(*this);
			new_id_set.ids.expand(id.id);
			new_id_set.ids.clear(id.id);
			return new_id_set;
		}
		component_ids with(const component_ids& other) const {
			return ids & other.ids;
		}
		component_ids with(component_id id) const{
			component_ids new_id_set =component_ids(*this);
			new_id_set.ids.expand(id.id);
			new_id_set.ids.set(id.id);
			return new_id_set;
		}
		//repr
		
		component_ids flipped(const component_ids& other) const {
			return ids ^ other.ids;
		}
		component_ids flipped(component_id id) const {
			component_ids new_id_set = component_ids(*this);
			new_id_set.ids.expand(id.id);
			new_id_set.ids.set(id.id);
			return new_id_set;
		}
		bool operator!=(const component_ids& other) const {
			return ids != other.ids;
		}
		bool operator==(const component_ids& other) const{
			return ids == other.ids;
		}
	};
	struct archetype_predicate {
		component_ids needed;
		component_ids wanted;
		archetype_predicate(const component_ids& wants, const component_ids& needs) :needed(needs), wanted(wants) {
			wanted= wanted.without(needed);

		}
		bool passes(component_ids ids) const{
			return ids.has(needed);
		}
	};

}