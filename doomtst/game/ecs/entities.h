#include "entity.h"
#include "../../util/dynamicarray.h"
#include "../../util/index.h"
#include "EntityAllocator.h"
#pragma once
namespace ecs {
	struct ArchetypeIndexTag {
	};
	using archetype_index = stn::typed_index<ArchetypeIndexTag>;
	//index of a distint archetypr
	struct Archetype_tag {
	};
	using archetype_id = stn::typed_id<Archetype_tag>;

	struct archetype_location {
		archetype_index index;
		archetype_id id;
		archetype_location(archetype_id arch_id, archetype_index  arch_index) :id(arch_id), index(arch_index) {
		}
		bool operator!=(const archetype_location& other) const = default;
		bool operator==(const archetype_location& other) const = default;
	};

	struct entity_metadata {
		uint32_t gen_count;
		entity_metadata(entity_id id_for):id(id_for) {
			gen_count = 0;
		}
		void clear() {
			gen_count++;
			is_alive = false;
		}
		entity make_live_handle() {
			is_alive = true;
			return entity(id, gen_count);
		}
		bool is_alive;
		entity_id id;
	};
	

	struct Entities {
		Entities() :entity_list(),free_ids() {

		}

		entity_metadata& operator[](size_t index) {	
			return entity_list[index];
		}
		const entity_metadata& operator[](size_t index)	const{
			return entity_list[index];
		}
		bool contains(entity entity) const{
			return entity_list.contains_index(entity.id().id)&&entity.generation() == entity_list.unchecked_at(entity.id().id).gen_count;
		}
		bool contains(entity_id entity) const {
			return entity_list.contains_index(entity.id)&&entity_list[entity.id].is_alive;
		}
		//for more expressive 
		entity_metadata& at(entity_id entity) {
			return entity_list[entity.id];
		}
		const entity_metadata& at(entity_id entity) const {
			return entity_list[entity.id];
		}
		entity_metadata& at(entity entity) {
			return entity_list[entity.id().id];
		}
		const entity_metadata& at(entity entity) const {
			return entity_list[entity.id().id];
		}
		void assert_valid(entity entity) const{
			if (!contains(entity))
			{
				stn::throw_logic_error("refrence to entity with block_id {} is a member_ref of outdated generation {} of {}", entity.id(), entity.generation(), entity_list[entity.id().id].gen_count);
			}
		}
		entity_metadata& operator[](entity_id entity) {
			return at(entity);
		}
		const entity_metadata& operator[](entity_id entity) const {
			return at(entity);
		}
		entity_metadata& operator[](entity entity) {
			return at(entity.id());
		}
		const entity_metadata& operator[](entity entity) const {
			return at(entity.id());
		}
		
		size_t length() const{
			return entity_list.length();
		}
		bool is_empty() const {
			return entity_list.empty();
		}
		template<typename A= default_allocator>
		entity allocate_entity() {
			stn::insertion<entity_id> id= free_ids.allocate<A>();
			if (id.is_new) {
				//expands to the index
				entity_list.geometric_expand_with(max_sharing_page(id.value).id + 1,
					[](size_t index) {
						return entity_metadata(entity_id(index));
					}
				);
			}
			return at(id.value).make_live_handle();
		}
		void remove_entity(entity entity) {
			at(entity.id()).clear();
			free_ids.free(entity.id());
		}
	private:

		EntityIds free_ids;
		stn::array<entity_metadata> entity_list;
	};
}