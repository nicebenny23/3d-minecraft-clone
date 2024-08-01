#include "entity.h"
#include "../world/grid.h"
using namespace entityname;
array<entity*> entityname::objectfromguid;
array<entity*> todelete;
array<entity*> initobj;
int entityname::totalcount;
bool entityname::shouldspawnfinalboss;
void entityname::initobjs() {
	totalcount = 0;
	objectfromguid = array<entity*>(entsize);
	for (int i = 0; i < entsize; i++)
	{
		objectfromguid[i] = nullptr;
	}
	todelete = array<entity*>();
	initobj = array<entity*>();

	shouldspawnfinalboss = false;

}

inline int getgoid() {
	int randomval;
	entity* valifexist;
	do {
		randomval = randomint(entsize - 1);
		valifexist = objectfromguid[randomval];
	} while (valifexist != nullptr);

	return randomval;


}

void entityname::deleteobjs()
{
	int len = todelete.length;
	for (int ind = 0; ind < len; ind++) {


		todelete[ind]->senddestroycall();



		objectfromguid[todelete[ind]->guid] = nullptr;

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

void entityname::destroy(entity* ent,bool soft)
{
	if (ent!=nullptr)
	{
		if (soft)
		{
			ent->state = gameobject::beingsoftdestroyed;
		}
		else
		{
			ent->state = gameobject::beingroughdestroyed;
		}
		todelete.append(ent);
	}
	
}
struct entitycomponentindexer
{
	entitycomponentindexer(int entind, int compnum) {

		entity = entind;
		componentnumber = compnum;
	}
	int entity;
	int componentnumber;
	gameobject::component* at() {

		return objectfromguid[entity]->complist[componentnumber];
	}
};

int comparecomponent(const void* b, const void* a) {
	
	int priorityb =(* (entitycomponentindexer*)(b)).at()->priority;

	int prioritya = (*(entitycomponentindexer*)(a)).at()->priority;
	return (prioritya>priorityb) - (priorityb> prioritya);
	
}
entityref entityname::createentity(v3::Vector3 ipos, const char* _name) {
	entity* object = new entity();
	object->type = gameobject::entity;
	
object->state = gameobject::beinginitiated;

	object->transform.position = ipos;

	object->guid = getgoid();

	object->name = _name;
	object->complist = (array<gameobject::component*>());

	object->tags = array<std::string*>();
	objectfromguid[object->guid] = object;
	object->addtag("entity");

	return entityref(*object);

}


void entityname::runupdateloop() {

	array<entitycomponentindexer> componentlist;
	
	for (int i = 0; i < objectfromguid.length; i++)
	{
		
		if (objectfromguid.at(i) != nullptr) {
			
			if (grid::getobjatgrid(objectfromguid.at(i)->transform.position,true)==nullptr)
			{
				Vector3 mm = camera::campos;
				Vector3 griddd = grid::gridpos;
				entity* ent = objectfromguid.at(i);
				
				
					if (objectfromguid.at(i)->canbedestroyed)
					{

						destroy(objectfromguid.at(i), false);

					}
				
			}



			int len = objectfromguid[i]->complist.length;

			for (int j = 0; j < len; j++)
			{
				if (objectfromguid[i]->complist[j]->active)
				{
					componentlist.append(entitycomponentindexer(i, j));

				}
				


			}


		}
	}
	std::qsort(componentlist.getdata(), componentlist.length, sizeof(entitycomponentindexer), comparecomponent);
	for (int i = 0; i < componentlist.length; i++)
	{
		if (componentlist[i].at()!= nullptr) {
			if (gameobject::shouldbeupdated(objectfromguid[componentlist[i].entity])){

				componentlist[i].at()->update();
			}
		}
	}
	componentlist.destroy();
}

void entityname::runrenderloop()
{
	for (int i = 0; i < objectfromguid.length; i++)
	{

		if (objectfromguid.at(i) != nullptr) {




			int len = objectfromguid[i]->complist.length;

			for (int j = 0; j < len; j++)
			{
	
				objectfromguid[i]->complist[j]->renderupdate();
			}
		}
	}
}
