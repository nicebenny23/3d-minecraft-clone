
#include "gameobject.h"

#include "GameContext.h"
#include "../util/random.h"
#include <type_traits>
#include "../renderer/Renderer.h"


using namespace gameobject;





void gameobject::component::ondestroy()
{
}


void gameobject::component::destroy()
{
	ondestroy();
	ctx->OC->managers[id].pool.free(this);

}

void gameobject::component::start()
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
	//Manager = &Man;

};
//gets a gameobject from a refrence to it;


void gameobject::OCManager::InitObj(obj* object)
{
	object->id = ObjId;
	ObjId++;
	object->OC = ctx->OC;

}

void gameobject::OCManager::updatecomponents(updatecalltype type)
{

	array<componentmanager*> managerref;

	for (int i = 0; i < managers.length; i++)
	{


		if (shouldupdate( managers[i].utype,type))
		{
			managerref.push(&managers[i]);
		}
		

	}
	std::sort(&managerref[0], &managerref[0]+managerref.length, [](componentmanager* a, componentmanager* b) {
		return a->priority>b->priority;
		});
	
	for (int j = 0; j < managerref.length; j++)
	{
		componentmanager* manager = managerref[j];
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


	pool = chainpool::chainedpool<component>(bytesize);
}

void gameobject::componentmanager::init(component* sample)
{
	//one method to remove this would involve iterating through all managers during updates

	priority = sample->priority;

	utype = sample->utype;
}


