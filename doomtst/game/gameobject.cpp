
#include "gameobject.h"


#include "../util/random.h"
#include <type_traits>
#include "../renderer/Renderer.h"





using namespace gameobject;

std::unordered_map<const char*, int> gameobject::stringtoint;
int gameobject::curid;



void gameobject::component::ondestroy()
{
}


void gameobject::component::start()
{
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
	state = beinginitiated;
	

};
array<componentmanager,true> gameobject::managerlist;
//gets a gameobject from a refrence to it;

void gameobject::initmanagerlist()
{
	managerlist = array<componentmanager,true>(0);
}

int comparemanager(const void* b, const void* a) {

	int priorityb = managerlist[*((int*)(b))].priority;

	int prioritya = managerlist[*((int*)(a))].priority;
	return (prioritya > priorityb) - (priorityb > prioritya);

}
void gameobject::updatecomponents()
{

	array<int> managerref;

	for (int i = 0; i < managerlist.length; i++)
	{


		if (shouldupdate( managerlist[i].utype))
		{
			managerref.append(i);
		}
		

	}

	std::qsort(managerref.list, managerref.length, sizeof(int), comparemanager);
	for (int j = 0; j < managerref.length; j++)
	{
		componentmanager* manager = &managerlist[managerref[j]];
		for (component& comps :manager->pool)
		{
			

				if (comps.active)
				{
					comps.update();
				}
			
		}
	}
	managerref.destroy();
}

componentmanager* gameobject::managerof(component* comp)
{
	
	return &managerlist.fastat(comp->id);
}

void gameobject::destroy(obj* object)
{
	for (int i = 0; i < object->componentlist.length; i++)
	{
		object->componentlist[i]->destroy();
	}

 	object->componentlist.destroy();
}

gameobject::componentmanager::componentmanager()
{
	
	id = -1;
	priority = -1;
	utype = updatenone;
	
}

void gameobject::componentmanager::create(int mid, int bytesize)
{

	id = mid;


	pool = chainpool::chainedpool<component>(bytesize, 10000);
}

void gameobject::componentmanager::init(component* sample)
{
	priority = sample->priority;

	utype = sample->utype;
}


