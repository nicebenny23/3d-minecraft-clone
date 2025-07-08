
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
	owner.OC->delete_component(this);

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
	return Id ? true : false;

}

EntityMetadata& gameobject::obj::meta()
{
	
	if (!Id)
	{
		throw std::logic_error("entity Has invalid id");
	}
	EntityMetadata& met = OC->entitymeta[Id.id]; 
	if (met.gen_count!=Id.gen)
	{
		throw std::logic_error("Cannot acess entity, It has been deleted");
	}
	return met;

}




//gets a gameobject from a refrence to it;

void OCManager::destroy(obj* object) {
	// 1) Mark destroying
	array<component*> comps;
	for (component* comp : ComponentView(*object)) {
		comp->destroy();
	}

	comps.destroy();
	object->meta().arch->remove(*object);
	

	
	entitymeta[object->Id.id].reset();
	free_ids.push(object->Id.id);
}
void gameobject::OCManager::Delete_deffered_objs()
{
	while (!EntityDeletionBuffer.empty())
	{
		gameobject::obj elem = EntityDeletionBuffer.pop();
		if (elem.hascomponent<StaticComponent>())
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
	if(comp->owner==None)
	{
		throw std::logic_error("Every Component Must be owned by a valid");
	}
	managers[comp->comp_id.value].store.erase_key(comp->owner.Id.id);

	managers[comp->comp_id.value].pool.free(comp);
}
void gameobject::OCManager::InitObj(obj& object)
{
	object.Id.id = free_ids.pop();
	object.OC = ctx->OC;
	object.Id.gen= entitymeta[object.Id.id].gen_count;

	arch.archtypes[0].add(object);

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
		for (Archtype& arch : arch.archtypes)
		{

			if (arch.has_component(manager->id))
			{


				for (obj comps : arch.elems)
				{
					component* comp = manager->store.getByKey(comps.Id.id);

					
						comp->update();
					

				}
			}
		}
	}
	managerref.destroy();
}

obj gameobject::OCManager::CreateEntity(v3::Vector3 SpawnPos)
{
	obj object = obj();
	InitObj(object);
	object.addcomponent<transform_comp>()->transform.position = SpawnPos;
	return object;
}




gameobject::componentmanager::componentmanager()
{
	
	id = comp::None;
	priority = -1;
	utype = updatenone;
	
}

void gameobject::componentmanager::create(comp::Id mid, int bytesize)
{
	id = mid;
	pool = dynPool::flux<component>(bytesize);
}

void gameobject::componentmanager::init(component* sample)
{
	priority = sample->priority;
	utype = sample->utype;
}


constexpr size_t gameobject::ComponentMapper::operator()(component* comp) const noexcept
{
	return comp->owner.Id.id;
}
