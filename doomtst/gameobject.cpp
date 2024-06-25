
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






int gameobject::getgoid()
{
	int randomval = 0;
	obj* valifexist = 0;

	while (valifexist != 0)
	{
		randomval = randomint(entsize-1);
		valifexist = objectfromguid[randomval];

	}

	return randomval;


}

array<obj*> gameobject::objectfromguid;

//todo move over to enties as 
array<obj*> delobjs;
array<obj*> initobj;
void gameobject::initobjs() {

	objectfromguid = array<obj*>(entsize);
	delobjs = array<obj*>();
	initobj = array<obj*>();



}

gameobjref init(gameobjref base) {



	obj clone = obj();

	for (int i = 1; i < base.toobj()->complist.length; i++)
	{


	}

	return gameobjref(clone);

}

//dont use
gameobject::obj::obj(v3::Vector3 ipos, const char* _name)
{
	
	
	
	guid = getgoid();

	complist = (array<component*>());
	
	addcomponent<component>();
	objectfromguid[guid] = this;





}



obj::obj() {


	complist.list = nullptr;
};




void gameobject::destroy(obj* object) {


	delobjs.append(object);
}

void gameobject::deleteobjs()
{
	int len = delobjs.length;
	for (int ind = 0;ind < len;ind++) {

		
		for (size_t i = 0; i < delobjs[ind]->complist.length; i++)
		{
			delobjs[ind]->complist[i]->ondestroy();


		}
		
		
		
		objectfromguid[delobjs[ind]->guid] = 0;
		
		
//	delete delobjs[ind];
	
	}
	if (delobjs.length > 0)
	{
		//deletes the pointers to the obkjects
	delobjs.destroy();
	
		delobjs = array<obj*>();
		delobjs.length = 0;
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


//gets a gameobject from a refrence to it;
obj* gameobject::gameobjref::toobj()
{
	if (guid<-1)
	{
		return nullptr;
	}
	obj* toreturn = objectfromguid[guid];
	if (toreturn == 0)
	{
		guid = 0;
	}
	return toreturn;
}
