
#include "gameobject.h"

#include "GameContext.h"
#include <type_traits>
#include <algorithm>


using namespace gameobject;





void gameobject::component::ondestroy()
{
}


void gameobject::component::destroy()
{
	ondestroy();
	ctx->OC->delete_component(this);

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



void gameobject::component::oncollision(obj collidedwith)
{
}







//todo move over to enties as 

//not functonal yet




bool gameobject::obj::exists() const
{
	return Id != 0;
}

EntityMetadata& gameobject::obj::meta()
{
	
	EntityMetadata& met = OC->entitymeta[Id]; 
		if (met.gen_count==gen)
		{

			return OC->entitymeta[Id];

		}
		throw std::logic_error("Cannot acess deleted entity");
}

array<component*>& gameobject::obj::componentlist()
{
	return meta().componentlist;
}



objstate& gameobject::obj::state()
{
	return meta().state;
}


//gets a gameobject from a refrence to it;


void gameobject::OCManager::destroy(obj* object)
{
	array< component*>& complist =object->componentlist();

	object->state() = destroying;
	for (int i = 0; i < complist.length; i++)
	{
		complist[i]->destroy();
	}

	complist.destroy();
	entitymeta[object->Id].reset();
	free_ids.push(object->Id);

}
void gameobject::OCManager::Delete_deffered_objs()
{
	while (!EntityDeletionBuffer.empty())
	{
		gameobject::obj elem = EntityDeletionBuffer.pop();
		if (!elem.hascomponent<StaticComponent>())
		{
			throw std::logic_error("cannot destroy this entity");
		}
		elem.immediate_destroy();
	}

} 
void obj::immediate_destroy()
{
	OC->destroy(this);
}
void gameobject::obj::deffered_destroy()
{
	OC->EntityDeletionBuffer.push(*this);
}
Transform& gameobject::obj::transform()
{
	return getcomponent<gameobject::transform_comp>().transform;
}
void gameobject::OCManager::delete_component(component* comp)
{
	managers[comp->comp_id].pool.free(comp);
}
void gameobject::OCManager::InitObj(obj* object)
{
	object->Id = free_ids.pop();
	object->OC = ctx->OC;
	object->gen= entitymeta[object->Id].gen_count;

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

obj gameobject::OCManager::CreateEntity(v3::Vector3 SpawnPos)
{
	obj* object = new obj();
	InitObj(object);

	object->componentlist() = array<gameobject::component*>();
	object->addcomponent<transform_comp>()->transform.position = SpawnPos;
	return *object;
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


