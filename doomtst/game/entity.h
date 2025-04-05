#include "../util/memorypool.h"
#include "gameobject.h"
#include "aabb.h"
#include "../util/vector3.h"
#include "transform.h"
#include <string>
#ifndef entity_HPP
#define entity_HPP

constexpr auto maxentities = 16 * 16 * 16;
inline std::string str(const char* charlist) {

	return 	std::string(charlist);
}
namespace entityname {
	extern bool shouldspawnfinalboss;


	extern int entityalloccounter;

	struct entity : gameobject::obj
	{

		bool canbedestroyed = false;
		const char* name;
		array<std::string*> tags;
		int amtcount;
		int index;
		void removetag(std::string tag);
		void addtag(std::string tag);
		bool hastag(std::string tag);
		Transform transform;
		entity() {
			
amtcount = entityalloccounter;
			entityalloccounter += 1;
			state = gameobject::beinginitiated;
		type = gameobject::entity;

		
		
		}
	};

	extern ptrmempool<entity> entitypool;

	struct entityref
	{
		entity* operator->() {

			return toent();
		}
		int sharedcount;
		int guid;
		entityref(entity& object) {
			guid = object.index;
			sharedcount = object.amtcount;
		}
		entity* toent()
		{
			if (guid == -1)
			{
				return nullptr;
			}
			
			entity* toreturn = (entity*)(entitypool[guid]);
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
		entity* toentnotnull() {

			entity* ent = toent();
			if (ent == nullptr)
			{
				Assert("ent cant be null");
			}
			return ent;
		}
	};


	void destroy(entity* ent);
	entityref createentity(v3::Vector3 ipos, const char* _name);
	
	void runrenderloop();
	void deleteobjs();
	void initobjs();
}

#endif entity_HPP