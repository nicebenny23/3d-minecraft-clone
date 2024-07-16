#include "gameobject.h"
#include "aabb.h"
#include "../util/vector3.h"
#define entsize 16*16*16*150
#ifndef entity_HPP
#define entity_HPP
namespace entityname {
	



	

	struct entity : gameobject::obj
	{
		const char* name;
		v3::Vector3 pos;

		int guid;

		entity() = default;
	};


	extern array<entity*> objectfromguid;
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
			entity* toreturn = (entity*)(objectfromguid[guid]);
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

	void destroy(entity* ent);
	entityref createentity(v3::Vector3 ipos, const char* _name);
	void runupdateloop();
	void deleteobjs();
	void initobjs();
}

#endif entity_HPP