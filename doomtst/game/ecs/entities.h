#include "entity.h"
#include "../../util/dynamicarray.h"
#include "../../util/index.h"
#pragma once
namespace ecs {
	struct ArchetypeIndexTag {};
	using archetype_index = stn::typed_index<ArchetypeIndexTag>;
	//index of a distint archetypr
	struct Archetype_tag {};
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
		entity_metadata() {
			gen_count = 0;
		}
		void clear() {
			gen_count++;

		}

	};
	struct Entities {
		Entities(size_t count) :entity_list(count) {

			for (std::uint32_t i = 0; i < count; i++) {
				free_ids.push(i);
			}
		}

		entity_metadata& operator[](size_t index) {	
			return entity_list[index];
		}
		const entity_metadata& operator[](size_t index)	const{
			return entity_list[index];
		}
		bool is_valid(entity entity) const{

			return entity.generation() == entity_list[entity.id()].gen_count;
		}
		//for more expressive 
		entity_metadata& at(entity entity) {
			return entity_list[entity.id()];
		}
		void assert_valid(entity entity) const{
			if (!is_valid(entity))
			{
				stn::throw_logic_error("refrence to entity with block_id {} is a member of outdated generation {} of {}", entity.id(), entity.generation(), entity_list[entity.id()].gen_count);
			}
		}
		const entity_metadata& at(entity entity) const{
			return entity_list[entity.id()];
		}
		entity_metadata& operator[](entity entity) {
			return at(entity);
		}
		const entity_metadata& operator[](entity entity) const {
			return at(entity);
		}
		
		size_t length() const{
			return entity_list.length();
		}
		bool is_empty() const {
			return entity_list.empty();
		}
		entity allocate_entity() {
			std::uint32_t id= free_ids.pop();
			return entity(id, entity_list[id].gen_count);
		}
		void remove_entity(entity entity) {
			at(entity).clear();
			free_ids.push(entity.id());
		}
	private:

		stn::array<uint32_t> free_ids;
		stn::array<entity_metadata> entity_list;
	};
}