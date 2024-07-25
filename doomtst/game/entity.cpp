#include "entity.h"
using namespace entityname;
array<entity*> entityname::objectfromguid;
array<entity*> todelete;
array<entity*> initobj;
int entityname::totalcount;
void entityname::initobjs() {
	totalcount = 0;
	objectfromguid = array<entity*>(entsize);
	for (int i = 0; i < entsize; i++)
	{
		objectfromguid[i] = nullptr;
	}
	todelete = array<entity*>();
	initobj = array<entity*>();



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

void entityname::destroy(entity* ent)
{
	todelete.append(ent);
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


	objectfromguid[object->guid] = object;


	return entityref(*object);

}


void entityname::runupdateloop() {

	array<entitycomponentindexer> componentlist;
	
	for (int i = 0; i < objectfromguid.length; i++)
	{

		if (objectfromguid.at(i) != nullptr) {




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
