
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
	ctx->OC->managers[comp_id].store.erase_key(owner.Id.id);
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
	return Id.id;
}

EntityMetadata& gameobject::obj::meta()
{
	
	EntityMetadata& met = OC->entitymeta[Id.id]; 
		if (met.gen_count==Id.gen)
		{

			return met;

		}
		throw std::logic_error("Cannot acess deleted entity");
}



objstate& gameobject::obj::state()
{
	return meta().state;
}


//gets a gameobject from a refrence to it;

void OCManager::destroy(obj* object) {
	// 1) Mark destroying
	object->state() = destroying;

	// 2) Remove it from its archetype
	Archtype* arch = object->meta().arch;
	if (arch) {
		arch->remove(*object);         // now it's no longer in elems
		object->meta().arch = nullptr; // clear dangling pointer
	}

	// 3) Snapshot component pointers
	array<component*> comps;
	for (component* comp : ComponentView(*object)) {
		comps.push(comp);
	}

	// 4) Destroy each component
	for (component* comp : comps) {
		comp->destroy();
	}
	comps.destroy();

	// 5) Finally, reset entity metadata
	entitymeta[object->Id.id].reset();
	free_ids.push(object->Id.id);
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
	object->Id.id = free_ids.pop();
	object->OC = ctx->OC;
	object->Id.gen= entitymeta[object->Id.id].gen_count;
	archtypes[0].add(*object);

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
		for (Archtype& arch : archtypes)
		{

			if (arch.has_component(manager->id))
			{


				for (obj comps : arch.elems)
				{
					component* comp = manager->store.getByKey(comps.Id.id);

					if (comp->active) 
					{
						comp->update();
					}

				}
			}
		}
	}
	managerref.destroy();
}

obj gameobject::OCManager::CreateEntity(v3::Vector3 SpawnPos)
{
	obj* object = new obj();
	InitObj(object);
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


	pool = dynPool::flux<component>(bytesize);
}

void gameobject::componentmanager::init(component* sample)
{
	//one method to remove this would involve iterating through all managers during updates

	priority = sample->priority;

	utype = sample->utype;
}


constexpr size_t gameobject::ComponentMapper::operator()(component* comp) const noexcept
{
	return comp->comp_id;
}
