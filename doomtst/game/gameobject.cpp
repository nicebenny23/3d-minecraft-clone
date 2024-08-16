
#include "gameobject.h"


#include "../util/random.h"
#include <type_traits>
#include "../renderer/Renderer.h"





using namespace gameobject;


//todo make my own hashmap
std::unordered_map<const char*, int> gameobject::stringtoint;
int gameobject::curid;



void gameobject::component::ondestroy()
{
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
	state = beinginitiated;
	

};
array<componentmanager> gameobject::managerlist;
//gets a gameobject from a refrence to it;

void gameobject::initmanagerlist()
{
	managerlist = array<componentmanager>(0, true);
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

	std::qsort(managerref.getdata(), managerref.length, sizeof(int), comparemanager);
	for (int j = 0; j < managerref.length; j++)
	{
		componentmanager* manager = &managerlist[managerref[j]];
		for (int i = 0; i < manager->componentlist.capacity; i++)
		{
			if (manager->componentlist[i]!=nullptr)
			{

				if (manager->componentlist[i]->active)
				{
					manager->componentlist[i]->update();
				}
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
	componentlist = array<component*>(0, true);
	id = -1;
	componentamt = -1;
	priority = -1;
	utype = updatenone;
	
}

void gameobject::componentmanager::create(int mid, int bytesize)
{

	id = mid;
	componentamt = 0;


	pool = dynamicmempool::dynamicpool(bytesize, 10000);
}

void gameobject::componentmanager::init(component* sample)
{
	priority = sample->priority;

	componentlist = array<component*>(0, true);
	utype = sample->utype;
}


void gameobject::componentmanager::append(component* comp)
{

	float componentdensisty =0;

	if (componentlist.capacity==0)
	{
		componentdensisty = 1;
	}
	else
	{
		componentdensisty = float(componentamt )/ float(componentlist.capacity);
	}

	//randomly chosen value
	if (.9f<componentdensisty)
	{
		comp->index = componentlist.capacity;
		componentlist[componentlist.capacity] = comp;
	}
	else {


	
			int testind;
		component* 	componentatval=nullptr;
			do {
				testind= randomint(componentlist.capacity);
				componentatval= componentlist[testind];
			} while (componentatval!= nullptr);
			componentatval = comp;
			comp->index = testind;
		
	}
	componentamt++;
}

void gameobject::componentmanager::remove(int id)
{
	componentlist[id] = nullptr;
	componentamt--;
}
