
#include "gameobject.h"

#include "GameContext.h"
#include <type_traits>
#include <algorithm>
#include "query.h"
#include "../util/random.h"
#include "../debugger/console.h"
#include "../player/playermovment.h"
using namespace gameobject;





void gameobject::component::destroy_hook()
{
}


void gameobject::component::destroy()
{
	destroy_hook();
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
	return entity&& OC->entitymeta[id()].gen_count == generation();

}


EntityMetadata& gameobject::obj::meta()
{
	
	if (!entity)
	{
		throw std::logic_error("entity does not have a valid id");
	}
	EntityMetadata& met = OC->entitymeta[id()];
	if (met.gen_count!=generation())
	{
		throw std::logic_error("Cannot acess entity, It has been deleted");
	}

	return met;

}
struct destroyCmd :Command {
	destroyCmd(obj Obj) {
		object = Obj;
	}
	obj object;
	void Apply(gameobject::Ecs* world) override {

		world->destroy(object);
	}
};


//gets a gameobject from a refrence to it;

void Ecs::destroy(obj& object) {

	if (!object.exists())
	{
	//to prevent double deletes
		return;
	}
	for (component* comp : query::ComponentView(object)) {
		comp->destroy();
	}
	arch[object.meta().arch].remove(object);
	entitymeta[object.id()].clear();
	free_ids.push(object.id());
}


void gameobject::obj::destroy()
{
	OC->commands.push(destroyCmd(*this));
}
Transform& gameobject::obj::transform()
{
	return getcomponent<gameobject::transform_comp>().transform;
}
void gameobject::Ecs::delete_component(component* comp)
{
	if(comp->owner==None)
	{
		throw std::logic_error("Every Component Must be owned by a entity");
	}
	component_table_with_id(comp->comp_id).remove_at(comp->owner);

}

obj gameobject::Ecs::spawn_empty()
{
	obj new_spawn;
	size_t new_id = free_ids.pop();
	new_spawn.entity =ecs::entity(new_id, entitymeta[new_id].gen_count);
	new_spawn.OC = this;

	arch.empty_archtype().add(new_spawn);
	return new_spawn;
}

void gameobject::Ecs::updatecomponents(updatecalltype type)
{

	array<component_table*> managerref;

	for (int i = 0; i < comp_storage.length(); i++)
	{
		
		if (shouldupdate( comp_storage[i].description.update_type,type))
		{
			
			//managerref.filer.
		managerref.push(&comp_storage[i]);
		}
		

	}
	if (!managerref.empty())
	{


		std::sort(&managerref[0], &managerref[0] + managerref.length(), [](component_table* a, component_table* b) {
			return a->description.priority > b->description.priority;
			});
	}
	for (component_table* mgr : managerref) {
		for (Archtype* archtype : arch) {
			
			if (!archtype->has_component(mgr->id)) continue;

			// 1) Snapshot the count _once_
			size_t originalCount = archtype->count();

			// 2) Index by integer, not range-for
			for (size_t i = 0; i < originalCount; ++i) {
				obj   o = (*archtype)[i];           // read by value
				auto* c = (*mgr)[o];
				c->update();  
				
			}
		}
	}
	
}



obj gameobject::Ecs::spawn_with_transform(v3::Vec3 SpawnPos)
{
	obj object = spawn_empty();
	object.addcomponent<transform_comp>()->transform.position = SpawnPos;
	return object;

}





void gameobject::component_table::init(component* sample)
{
	description = component_description(sample->priority, sample->utype);
}
