#include "gameobject.h"
#include "../util/SparseSet.h"
#pragma once

struct child:gameobject::component {
	
	
	bool is_child(const gameobject::obj& other) const {
		return target_entity == other;
	}

	gameobject::obj get_parent() {
		return target_entity;
	}
private:
	friend struct relationship_target;
	void destroy_hook() {
		target_entity.getcomponent<parent>().pop(owner);
	}
	child(gameobject::obj& entity) {

		target_entity = entity;
	}
	
private:
	gameobject::obj target_entity;

};
struct parent:gameobject::component {
	array < gameobject::obj> get_children() {
		return array(holders);
	}
	bool is_child(const gameobject::obj& source) const {
		return holders.contains(source);
	}
	parent(gameobject::obj& child_entity) {
		push(child_entity);
	}


	
		friend struct child;
	void push(gameobject::obj& relator) {
		if (owner==relator)
		{
			throw std::logic_error("Relationship is unable to point to itself");
		}
		if (holders.contains(relator))
		{
			return;
		}
		relator.addcomponent<child>(owner);
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
			Object.destroy();
		}


	}
	array<gameobject::obj> holders;

};