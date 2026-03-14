#include "entity.h"
#include "../../util/type_index.h"
#pragma once
namespace ecs {

	template<typename T>
	struct allocator_tag {

	};
	struct allocator_id_tag {

	};

	using default_allocator = allocator_tag<allocator_id_tag>;
	using allocator_id = stn::typed_id<allocator_id_tag >;


	struct page_id_tag {
	};
	using page_index = stn::typed_index< page_id_tag>;
	static constexpr size_t page_size = 8;

	static constexpr size_t page_elements = 1 << page_size;

	static constexpr size_t page_elements_minus_one=page_elements-1;
	inline page_index page_index_from_entity_id(entity_id id) {
		return page_index(id.id >> page_size);
	}
	inline entity_id max_sharing_page(entity_id id) {
		return entity_id((page_index_from_entity_id(id).index<<page_size)+page_elements_minus_one);
	}

	struct AllocatorPages {
		AllocatorPages() = default;
		stn::array<entity_id> free_ids;
		bool out_of_ids() const {
			return free_ids.empty();
		}
		void add_ids_for_page(page_index id) {
			size_t min_id = id.index * page_elements;
			for (size_t i = 0; i < page_elements; i++) {
				push(entity_id(std::uint32_t(i + min_id)));
			}
		}
		void push(entity_id id) {
			free_ids.push(id);
		}
		entity_id pop() {
			return free_ids.pop();
		}


	};
	//allocates entities segregated per page
	struct EntityIds {
		EntityIds() :current_page(0) {

		}
		stn::type_indexer<allocator_id> allocator_id_map;
		stn::array<AllocatorPages> allocators;
		stn::array<allocator_id> page_to_allocator;
		page_index current_page;
		AllocatorPages& allocator_from_id(allocator_id id) {
			return allocators[id.id];
		}
		const AllocatorPages& allocator_from_id(allocator_id id) const {
			return allocators[id.id];
		}
		AllocatorPages& allocator_from_page_index(page_index index) {
			return allocator_from_id(page_to_allocator[index.index]);
		}
		const AllocatorPages& allocator_from_page_index(page_index index) const {
			return allocator_from_id(page_to_allocator[index.index]);
		}
		AllocatorPages& allocator_from_entity_id(entity_id id) {
			return allocator_from_page_index(page_index_from_entity_id(id));
		}
		const AllocatorPages& allocator_from_entity_id(entity_id id) const {
			return allocator_from_page_index(page_index_from_entity_id(id));
		}
		void gen_page_for(allocator_id id) {
			//push a new page when out of ids
			page_to_allocator.push(id);
			allocator_from_id(id).add_ids_for_page(current_page++);
		}


		void free(entity_id id) {
			AllocatorPages& page = allocator_from_entity_id(id);
			page.push(id);
		}
		//returns the id along with whether the amount of entities was expanded
		template<typename A>
		allocator_id insert_allocator() {
			return allocator_id_map.insert<A>().on_insert([&](allocator_id id) {
				allocators.emplace();
				}).value;
		}


		template<typename A>
		stn::insertion<entity_id> allocate() {
			return allocate(insert_allocator<A>());
		}
		stn::insertion<entity_id> allocate(allocator_id allocator) {
			bool needed_expanding = false;
			AllocatorPages& alloc_pages = allocator_from_id(allocator);
			if (alloc_pages.out_of_ids()) {
				needed_expanding = true;
				gen_page_for(allocator);
			}
			return stn::insertion(alloc_pages.pop(), needed_expanding);
		}
	};

}