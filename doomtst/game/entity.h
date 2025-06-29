#include "../util/memorypool.h"
#include "gameobject.h"
#include "aabb.h"
#include "../util/stack.h"
#include "../util/vector3.h"
#include "transform.h"
#include <string>
#ifndef entity_HPP
#define entity_HPP

constexpr auto maxentities = 16 * 16 * 16;
inline std::string str(const char* charlist) {

	return 	std::string(charlist);
}
namespace CtxName {
	struct Context;
}
namespace Ent {
	
	 struct entity;
	 struct entityref;
	struct EntityManager
	{

		EntityManager();


		ptrmempool<entity> entitypool;
		stackname::stack<entity*> EntityDeletionBuffer;
		
		void DeleteObjs();
		entityref CreateEntity(v3::Vector3 SpawnPos, const char* _name);
		CtxName::Context* ctx;
	};

	struct entity : gameobject::obj
	{
		
		bool canbedestroyed ;
		const char* name;
		array<std::string*> tags;

		int index;
		void removetag(std::string tag);
		void addtag(std::string tag);
		bool hastag(std::string tag);
		Transform transform;
		entity() {
		tags = array<std::string*>();
	
		type = gameobject::entity;
		index = -1;
		canbedestroyed = false;
		EntManager = nullptr;
		
		}
		void setEntityManager(EntityManager* EntController) {
			EntManager = EntController;
			
		}
		void Destroy();
	
		EntityManager* EntManager;
		
	};



	struct entityref
	{
		entity* operator->() {

			return toent();
		}
		int id;
		int guid;
		entityref(entity& object) {
			guid = object.index;
			id = object.id;
			EntMan = object.EntManager;
		}
		entity* toent();
		
		entityref() {
			EntMan = nullptr;
			id = 0;
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
	private:
		EntityManager* EntMan;
	};



	
	
}

#endif entity_HPP