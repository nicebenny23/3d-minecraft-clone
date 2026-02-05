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
			is_alive = false;
		}
		void mark_alive() {
			is_alive = true;
		}
		bool is_alive;

	};
	struct Entities {
		Entities(size_t count) :entity_list(count) {

			for (std::uint32_t i = 0; i < count; i++) {
				free_ids.push(entity_id(i));
			}
		}

		entity_metadata& operator[](size_t index) {	
			return entity_list[index];
		}
		const entity_metadata& operator[](size_t index)	const{
			return entity_list[index];
		}
		bool contains(entity entity) const{
			return entity.generation() == entity_list[entity.id().id].gen_count;
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
				stn::throw_logic_error("refrence to entity with block_id {} is a member of outdated generation {} of {}", entity.id(), entity.generation(), entity_list[entity.id().id].gen_count);
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
		entity allocate_entity() {
			entity_id id= free_ids.pop();
			at(id).mark_alive();
			return entity(id, entity_list[id.id].gen_count);
		}
		void remove_entity(entity entity) {
			at(entity.id()).clear();
			free_ids.push(entity.id());
		}
	private:

		stn::array<entity_id> free_ids;
		stn::array<entity_metadata> entity_list;
	};
}