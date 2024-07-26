
#include "gameobject.h"
#include "aabb.h"
#include "../util/vector3.h"
#include "transform.h"
#include <string>
#define entsize 16*16*16
#ifndef entity_HPP
#define entity_HPP
#define playertag 3
inline std::string str(const char* charlist) {

	return 	std::string(charlist);
}
namespace entityname {



	extern int totalcount;

	struct entity : gameobject::obj
	{
		const char* name;
		array<std::string*> tags;
		int amtcount;
		int guid;
		void removetag(std::string tag);
		void addtag(std::string tag);
		bool hastag(std::string tag);
		Transform transform;
		entity() {
			
amtcount = totalcount;
			totalcount += 1;
			state = gameobject::beinginitiated;

		
		}
	};


	extern array<entity*> objectfromguid;
	struct entityref
	{
		entity* operator->() {

			return toent();
		}
		int sharedcount;
		int guid;
		entityref(entity& object) {
			guid = object.guid;
			sharedcount = object.amtcount;
		}
		entity* toent()
		{
			if (guid == -1)
			{
				return nullptr;
			}
			
			entity* toreturn = (entity*)(objectfromguid[guid]);
			if (toreturn==nullptr)
			{
				guid = -1;
				return nullptr;
			}
			if (toreturn->amtcount!=sharedcount)
			{
				guid = -1;

				return nullptr;
			}
			return toreturn;
		};
		entityref() {

			guid = -1;
		}
	};


	void destroy(entity* ent,bool soft =true);
	entityref createentity(v3::Vector3 ipos, const char* _name);
	void runupdateloop();
	void runrenderloop();
	void deleteobjs();
	void initobjs();
}

#endif entity_HPP