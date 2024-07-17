#include "entity.h"
using namespace entityname;
array<entity*> entityname::objectfromguid;
array<entity*> todelete;
array<entity*> initobj;
void entityname::initobjs() {

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


		for (size_t i = 0; i < todelete[ind]->complist.length; i++)
		{
			todelete[ind]->complist[i]->ondestroy();


		}



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

entityref entityname::createentity(v3::Vector3 ipos, const char* _name) {
	entity* object = new entity();
	object->type = gameobject::entity;


	object->pos = ipos;

	object->guid = getgoid();

	object->name = _name;
	object->complist = (array<gameobject::component*>());


	objectfromguid[object->guid] = object;


	return entityref(*object);

}


void entityname::runupdateloop() {



	for (int i = 0; i < objectfromguid.length; i++)
	{

		if (objectfromguid.at(i) != nullptr) {




			int len = objectfromguid[i]->complist.length;

			for (int j = 0; j < len; j++)
			{


				objectfromguid[i]->complist[j]->update();



			}


		}
	}

}