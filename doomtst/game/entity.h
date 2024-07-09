#include "gameobject.h"
#include "aabb.h"
#include "../util/vector3.h"

#ifndef entity_HPP
#define entity_HPP
namespace entity {
	struct entity;
	


	struct entity : gameobject::obj
	{
		const char* name;
		v3::Vector3 pos;
	};




	struct entityref
	{
		int guid;

		entityref(entity& object) {
			guid = object.guid;
		}
		entity* toent()
		{
			if (guid == -1)
			{
				return nullptr;
			}
			entity* toreturn = (entity*)(gameobject::objectfromguid[guid]);
			if (false)
			{
				guid = -1;
			}
			return toreturn;
		};
		entityref() {

			guid = -1;
		}
	};


	inline entityref createentity(v3::Vector3 ipos, const char* _name) {
		entity* object = new entity();
		object->type = gameobject::entity;


		object->pos = ipos;

		object->guid = gameobject::getgoid();

		object->name = _name;
		object->complist = (array<gameobject::component*>());
		
		object->addcomponent<gameobject::component>();
		gameobject::objectfromguid[object->guid] = object;


		return entityref(*object);

	}
	

}

#endif entity_HPP