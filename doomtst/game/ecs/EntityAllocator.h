#include "entity.h"
#include "../../util/type_index.h"
#pragma once
namespace ecs {



	template<typename T>
	struct allocator_tag {

	};
	struct allocator_id_tag {

	};
	using allocator_id = stn::typed_id<allocator_id_tag >;
	
	struct EntityIdPage {
		stn::array<entity_id> free_ids;
		bool any_free() const {
			return free_ids.non_empty();
		}
		entity_id pop() {
			return free_ids.pop();
		}

		stn::Option<allocator_id> owner;
	};
	struct AllocatorSet {
		stn::array<EntityIdPage> with_some;
		stn::array<EntityIdPage> full;
	};
	struct EntityIdAllocator {
		stn::type_indexer<allocator_id> pages;
		stn::array<EntityIdPage> pages;
		stn::array<size_t> free_pages;

	};

}