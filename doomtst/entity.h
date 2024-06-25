#include "gameobject.h"
#include "aabb.h"
#include "vector3.h"
using namespace gameobject;
#ifndef entity_HPP
#define entity_HPP
namespace entity {
	struct entity : gameobject::obj
	{
		const char* name;
		v3::Vector3 pos;
	};
	gameobjref createentity(v3::Vector3 ipos, const char* _name) {
		entity* object = new entity();
		object->type = gameobject::entity;


		object->pos = ipos;

		object->guid = getgoid();

		object->name = _name;
		object->complist = (array<component*>());
		
		object->addcomponent<component>();
		objectfromguid[object->guid] = object;


		return gameobjref(*object);

	}
}

#endif entity_HPP