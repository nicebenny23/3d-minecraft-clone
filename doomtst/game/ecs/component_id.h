#include "entities.h"
#include "../../util/bit_mask.h"
#pragma once
namespace ecs {
	struct component_id_tag {};
	using component_id = stn::typed_id<component_id_tag>;
	using component_ids = stn::ids<component_id_tag>;
	
	struct archetype_predicate {
		component_ids needed;
		component_ids wanted;
		archetype_predicate(const component_ids& wants, const component_ids& needs) :needed(needs), wanted(wants) {
			wanted = wanted.without(needed);

		}
		bool passes(component_ids ids) const {
			return ids.has(needed);
		}
	};

}