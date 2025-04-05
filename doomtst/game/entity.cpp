#include "entity.h"
#include "../world/voxeltraversal.h"
#include "../world/grid.h"
using namespace entityname;
ptrmempool<entity> entityname::entitypool;
array<entity*> todelete;
array<entity*> initobj;
int entityname::entityalloccounter;
bool entityname::shouldspawnfinalboss;
void entityname::initobjs() {
	entityalloccounter = 0;

	todelete = array<entity*>();
	initobj = array<entity*>();
	entitypool.instantiate(maxentities);
	shouldspawnfinalboss = false;

}
ptrmempool<entity> objectguid;

void entityname::deleteobjs()
{
	int len = todelete.length;
	for (int ind = 0; ind < len; ind++) {


		todelete[ind]->senddestroycall();


		entitypool.remove(todelete[ind]);
	

		todelete[ind]->tags.destroy();
		//	delete delobjs[ind];

	}
	if (todelete.length > 0)
	{
		//deletes the pointers to the obkjects
		todelete.destroy();

		todelete = array<entity*>();
		todelete.length = 0;
	}

}

void entityname::entity::removetag(std::string tag)
{
	for (int i = 0; i < tags.length; i++)
	{
		if (*tags[i] == tag)
		{
			tags.deleteind(i);
			i--;
		}
	}
	
	Assert("entity must have tags to delete");
}

void entityname::entity::addtag(std::string tag)
{
	if (!hastag(tag))
	{
		std::string* newstring = new std::string(tag);
		tags.append(newstring);
	}
	else
	{
		Assert("entity already has tag");
	}
}

bool entityname::entity::hastag(std::string tag)
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

void entityname::destroy(entity* ent)
{
	if (ent!=nullptr)
	{
		
	
		todelete.append(ent);
	}
	
}


entityref entityname::createentity(v3::Vector3 ipos, const char* _name) {
	entity* object = new entity();
	
	object->transform.position = ipos;


	object->name = _name;
	object->componentlist = (array<gameobject::component*>());
	object->tags = array<std::string*>();

	object->addtag("entity");

	entitypool.append(object);
	
	return entityref(*object);

}



void entityname::runrenderloop()
{
	for (int i = 0; i < entitypool.size; i++)
	{

		if (entitypool[i] != nullptr) {




			int len = entitypool[i]->componentlist.length;

			for (int j = 0; j < len; j++)
			{
	
				entitypool[i]->componentlist[j]->renderupdate();
			}
		}
	}
}
