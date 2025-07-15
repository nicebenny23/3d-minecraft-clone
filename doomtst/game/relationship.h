#include "gameobject.h"
#include "../util/SparseSet.h"
#pragma once
struct Relation_Edge{
	Ids::Id start;
	Ids::Id end;
	size_t relation_id;
};
template<typename T>
struct relationship:gameobject::component {
	
	relationship<T>(const gameobject::obj& target_object) :target_entity(target_object) {

	}
	bool related_to(const gameobject::obj& other) const {
		return target_entity == other;
	}

	obj target() {
		return target_entity;
	}
private:
	friend relationship_target<T>
	void start() {

		if (owner == target_entity)
		{
			throw std::logic_error("Relationship is unable to point to itself");
		}
		target.addcomponent<relationship_target<T>>()->push(owner);
	}
	void destroy_hook() {
		target_entity.getcomponent<relationship_target<T>>().pop(owner);
	}
	

private:
	gameobject::obj target_entity;

};
template<typename T>
struct relationship_target :gameobject::component {
	sources() {
		return holders;
	}
	bool is_source(const gameobject::obj& source) const {
		return holders.contains(source);
	}

	relationship_target():holders() {
	}
	private:
	friend struct relationship<T>;
	void push(gameobject::obj& relator) {
		if (holders.contains(relator))
		{
			return;
		}
		holders.push(relator);
	}
	void pop(gameobject::obj& relator) {
		for (size_t i = 0; i < holders.length; i++)
		{
			if(holders[i] == relator) {
				holders.deleteind(i);

			}
		}
	}
	void destroy_hook() {
		for (auto& Object:holders )
		{
			Object.removecomponent<relationship<T>>();
		}


	}
	array<gameobject::obj> holders;

};