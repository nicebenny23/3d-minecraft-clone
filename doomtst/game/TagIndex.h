#pragma once
#include <unordered_map>
#include <typeindex>
struct Archetype{


};
struct ArchetypeSystem {
	size_t max_id;
	std::unordered_map<std::type_index, size_t> CompHasher;
	size_t GetCompIdAdd(std::type_index type) {
		int compId = CompHasher[type];
		if (compId == 0)
		{
			CompHasher[type] = max_id;
			max_id++;
			return CompHasher[type];
		}
		return compId;
	}

	size_t GetCompId(std::type_index type){
		int compId = CompHasher[type];
		if (compId == 0)
		{
			return -1;
		}
		return compId;
	}
};