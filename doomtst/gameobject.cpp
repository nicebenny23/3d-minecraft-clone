
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
	component newcomp =*(new component());
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
	obj* valifexist = objectfromguid[randomval];

	while (valifexist != nullptr)
	{
		
		randomval = randomint(entsize-1);
		valifexist = objectfromguid[randomval];
		int l = 1;

	}

	return randomval;


}

array<obj*> gameobject::objectfromguid;

//todo move over to enties as 
array<obj*> todelete;
array<obj*> initobj;
void gameobject::initobjs() {

	objectfromguid = array<obj*>(entsize);
	for (int i = 0; i < entsize; i++)
	{
		objectfromguid[i] = nullptr;
	}
	todelete = array<obj*>();
	initobj = array<obj*>();



}

objref init(objref base) {



	obj clone = obj();

	for (int i = 1; i < base.toobj()->complist.length; i++)
	{


	}

	return objref(clone);

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


	todelete.append(object);
}

void gameobject::deleteobjs()
{
	int len = todelete.length;
	for (int ind = 0;ind < len;ind++) {

		
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
	
		todelete = array<obj*>();
		todelete.length = 0;
	}

}


void gameobject::runupdateloop() {



	for (int i = 0; i < objectfromguid.length; i++)
	{
		if (objectfromguid.at(i) != 0) {
			int len = objectfromguid[i]->complist.length;
			
			for (int j = 0; j < len; j++)
			{

			
				objectfromguid[i]->complist[j]->update();
			}
		}
		
	}
}


//gets a gameobject from a refrence to it;
bool guidchanged(objref* ref) {

	return false;
}
//todo add a special random number to each gameobject
obj* gameobject::objref::toobj()
{
	if (guid==-1)
	{
		return nullptr;
	}
	obj* toreturn = objectfromguid[guid];
	if (guidchanged(this))
	{
		guid = -1;
	}
	return toreturn;
}
