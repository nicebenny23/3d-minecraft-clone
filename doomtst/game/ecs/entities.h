#include "entity.h"
#include "../../util/dynamicarray.h"
#include "../../util/index.h"
#pragma once
namespace ecs {
	struct ArchtypeIndexTag {};
	using archetype_index = stn::typestate::typed_index<ArchtypeIndexTag>;
	//index of a distint archetypr
	struct Archtype_tag {};
	using archetype_id = Ids::typed_id<Archtype_tag>;
	
	struct archetype_location {
		archetype_index index;
		archetype_id id;
		archetype_location() :id(0), index(0) {}
		archetype_location(size_t arch_id, size_t arch_index):id(arch_id),index(arch_index) {
			
		}
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
			if (!is_valid(entity))
			{
				throw stn::make_range_exception("refrence to entity with id {} is a member of outdated generation {} of {}",entity.id(),entity.generation(),entity_list[entity.id()].gen_count);
			}
			return entity_list[entity.id()];
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
		stn::array<size_t> free_ids;
		stn::array<entity_metadata> entity_list;
		entity allocate_entity() {
			size_t id= free_ids.pop();

			return entity(id, entity_list[id].gen_count);
		}
		void remove_entity(entity entity) {
			at(entity).clear();
			free_ids.push(entity.id());
		}
		Entities(size_t count){

			for (size_t i = 0; i < count; i++)
			{
				free_ids.push(i);
			}
		}
	};
}