
#include "gameobject.h"
#include "vector2.h"

#include "random.h"
#include <type_traits>
#include "Renderer.h"




using namespace gameobject;


//todo make my own hashmap
std::unordered_map<char*, int> gameobject::stringtoint;
int gameobject::curid;



void gameobject::component::ondestroy()
{
}

component* gameobject::component::copydat(component* orgin)
{
	component newcomp = component();
	return &newcomp;

}

void gameobject::component::start()
{

}
void gameobject::component::update()
{

}



array<gameobj*> objectfromguid;



int getgoid()
{
	int randomvalg = randomint(8000);
	gameobj* valifexist = objectfromguid[randomvalg];


	while (valifexist != 0)
	{
		randomvalg = randomint(8000);
		valifexist = objectfromguid[randomvalg];

	}

	return randomvalg;


}




array<gameobj*> delobjs;

array<gameobj*> initobj;
int s = 3;
void gameobject::initobjs() {

	objectfromguid = array<gameobj*>(8000);
	delobjs = array<gameobj*>();
	initobj = array<gameobj*>();



}

gameobjref init(gameobjref base) {



	gameobj clone = gameobj();

	for (int i = 1; i < base.obj()->complist.length; i++)
	{


	}

	return gameobjref(clone);

}
gameobjref gameobject::gameinit(Vector3 ipos, const char* _name) {
	gameobj* obj = new gameobj();
	

	
	obj->pos = ipos;

	obj->guid = getgoid();

	obj->name = _name;
	obj->complist = (array<component*>());
	s = obj->guid;
	obj->addcomponent<component>();
	objectfromguid[obj->guid] = obj;


	return gameobjref(*obj);

}
//dont use
gameobject::gameobj::gameobj(Vector3 ipos, const char* _name)
{
	tsty = false;
	
	pos = ipos;

	guid = getgoid();

	name = _name;
	complist = (array<component*>());
	s = guid;
	addcomponent<component>();
	objectfromguid[guid] = this;





}



gameobj::gameobj() {

	tsty = true;
	complist.list = nullptr;
};




void gameobject::destroy(gameobj* obj) {


	int s = delobjs.length;
	delobjs.append(obj);
	s = delobjs.length;
}

void gameobject::deleteobjs()
{

	for (int ind = 0;ind < delobjs.length;ind++) {


		objectfromguid[(delobjs[ind])->guid] = 0;
		for (size_t i = 0; i < delobjs[ind]->complist.length; i++)
		{
			delobjs[ind]->complist[i]->ondestroy();


			delete delobjs[ind]->complist[i];
		}

		delete[] delobjs[ind]->complist.getdata();

		delete delobjs[ind];

	}
	if (delobjs.length > 0)
	{
		delobjs.destroy();
		delobjs.length = 0;
		delobjs = array<gameobj*>();

	}

}


void gameobject::runupdateloop() {



	for (int i = 0; i < objectfromguid.length; i++)
	{
		if (objectfromguid.at(i) != 0) {
			int len = objectfromguid[i]->complist.length;

			for (int j = 0; j < len; j++)
			{

				component* l = objectfromguid[i]->complist[j];
				l->update();
			}
		}
	}
}


bool ltcomp(component* a, component* b) {

	return(a->id < b->id);

}

bool gtcomp(component* a, component* b) {

	return(a->id > b->id);

}

gameobj* gameobject::gameobjref::obj()
{

	gameobj* der = objectfromguid[guid];
	if (der == 0)
	{
		guid = 0;
	}
	return der;
}
