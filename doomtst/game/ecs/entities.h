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
		//returns the archetype at the next entry
		archetype_location next_archetype() const{
		return archetype_location(archetype_id(id.id + 1), archetype_index(0));
		}
		bool operator!=(const archetype_location& other) const = default;
		bool operator==(const archetype_location& other) const = default;
	};

	struct space_id_metadata {
		uint32_t gen_count;
		space_id_metadata() {
			gen_count = 0;
		}
		void clear() {
			gen_count++;

		}

	};
	struct Entities {
		space_id_metadata& operator[](size_t index) {	
			return space_id_list[index];
		}
		const space_id_metadata& operator[](size_t index)	const{
			return space_id_list[index];
		}
		bool is_valid(space_id space_id) const{

			return space_id.generation() == space_id_list[space_id.id()].gen_count;
		}
		//for more expressive 
		space_id_metadata& at(space_id space_id) {
			return space_id_list[space_id.id()];
		}
		void assert_valid(space_id space_id) const{
			if (!is_valid(space_id))
			{
				stn::throw_logic_error("refrence to space_id with id {} is a member of outdated generation {} of {}", space_id.id(), space_id.generation(), space_id_list[space_id.id()].gen_count);
			}
		}
		const space_id_metadata& at(space_id space_id) const{
			return space_id_list[space_id.id()];
		}
		space_id_metadata& operator[](space_id space_id) {
			return at(space_id);
		}
		const space_id_metadata& operator[](space_id space_id) const {
			return at(space_id);
		}
		
		size_t length() const{
			return space_id_list.length();
		}
		bool is_empty() const {
			return space_id_list.empty();
		}
		stn::array<uint32_t> free_ids;
		stn::array<space_id_metadata> space_id_list;
		space_id allocate_space_id() {
			std::uint32_t id= free_ids.pop();

			return space_id(id, space_id_list[id].gen_count);
		}
		void remove_space_id(space_id space_id) {
			at(space_id).clear();
			free_ids.push(space_id.id());
		}
		Entities(std::uint32_t count):space_id_list(count){

			for (std::uint32_t i = 0; i < count; i++)
			{
				free_ids.push(i);
			}
		}
	};
}