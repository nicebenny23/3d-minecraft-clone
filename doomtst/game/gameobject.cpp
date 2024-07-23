
#include "gameobject.h"


#include "../util/random.h"
#include <type_traits>
#include "../renderer/Renderer.h"





using namespace gameobject;


//todo make my own hashmap
std::unordered_map<char*, int> gameobject::stringtoint;
int gameobject::curid;



void gameobject::component::ondestroy()
{
}

component* gameobject::component::copydat(component* orgin)
{
	component newcomp = *(new component());
	return &newcomp;

}

void gameobject::component::start()
{
	priority = 0;
}
void gameobject::component::renderupdate()
{
}
void gameobject::component::update()
{

}

void gameobject::component::onplayerclick()
{
}



void gameobject::component::oncollision(obj* collidedwith)
{
}







//todo move over to enties as 

//not functonal yet


obj::obj() {

	complist = array<component*>();


};

//gets a gameobject from a refrence to it;

void gameobject::destroy(obj* object)
{
	for (int i = 0; i < object->complist.length; i++)
	{
		object->complist[i]->ondestroy();
	}

 	object->complist.destroy();
}