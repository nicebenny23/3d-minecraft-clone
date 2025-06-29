#include "entity.h"
#include "../world/voxeltraversal.h"
#include "../world/grid.h"
using namespace Ent;


Ent::EntityManager::EntityManager()
{


	EntityDeletionBuffer = stackname::stack<entity*>();

	entitypool.instantiate(maxentities);


}
void Ent::EntityManager::DeleteObjs()
{
	while (!EntityDeletionBuffer.empty())
	{
		entity* EntityToDestroy= EntityDeletionBuffer.pop();
		if (!EntityToDestroy->canbedestroyed)
		{
			throw std::logic_error("cannot destroy this entity");
		}
		EntityToDestroy->DestroyComponents();
		entitypool.remove(EntityToDestroy);
		EntityToDestroy->tags.destroy();
	}

	
}

void Ent::entity::removetag(std::string tag)
{
	bool hasTag = false;
	for (int i = 0; i < tags.length; i++)
	{
		if (*tags[i] == tag)
		{
			hasTag = true;
			tags.deleteind(i);
			i--;
		}
	}
	if (!hasTag) {


		warn("Entity Does not have tag");
	}
}

void Ent::entity::addtag(std::string tag)
{
	if (!hastag(tag))
	{
		std::string* newstring = new std::string(tag);
		tags.push(newstring);
	}
	else
	{
		Assert("entity already has tag");
	}
}

bool Ent::entity::hastag(std::string tag)
{
	for (int i = 0; i <tags.length ; i++)
	{
		if (*tags[i]==tag)
		{
			return true;
		}
	}
	return false;
}

void Ent::entity::Destroy()
{
	
	if (state==gameobject::destroying)
	{
		return;
	}
		state = gameobject::destroying;

		EntManager->EntityDeletionBuffer.push(this);
	
	
}


entityref Ent::EntityManager::CreateEntity(v3::Vector3 ipos, const char* _name) {
	entity* object = new entity();
	object->setEntityManager(this);
	ctx->OC->InitObj(object);
	object->transform.position = ipos;
	object->name = _name;
	object->componentlist = (array<gameobject::component*>());
	object->tags = array<std::string*>();
	entitypool.push(object);
	return entityref(*object);

}

entity* Ent::entityref::toent()
{
	if (guid == -1)
	{
		return nullptr;
	}

	entity* RefrencedEntity = (entity*)(EntMan->entitypool[guid]);
	if (RefrencedEntity == nullptr)
	{
		guid = -1;
		return nullptr;
	}
	if (RefrencedEntity->id != id)
	{
		guid = -1;

		return nullptr;
	}
	return RefrencedEntity;
}
