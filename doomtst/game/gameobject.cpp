
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
	return (Id ? true : false) && OC->entitymeta[Id.id].gen_count == Id.gen;

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
		return;
	}
	for (component* comp : query::ComponentView(object)) {
		comp->destroy();
	}

	object.meta().arch->remove(object);
	

	
	entitymeta[object.Id.id].reset();
	free_ids.push(object.Id.id);
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
		throw std::logic_error("Every Component Must be owned by a valid");
	}
	managers[comp->comp_id.value].store[comp->owner.Id.id]=nullptr;

	managers[comp->comp_id.value].pool.free(comp);
}
void gameobject::Ecs::InitObj(obj& object)
{
	object.Id.id = free_ids.pop();
	object.OC = ctx->OC;
	object.Id.gen= entitymeta[object.Id.id].gen_count;

	arch.archtypes[0]->add(object);

}

void gameobject::Ecs::updatecomponents(updatecalltype type)
{

	array<component_table*> managerref;

	arch.check();
	for (int i = 0; i < managers.length; i++)
	{
		
		if (shouldupdate( managers[i].utype,type))
		{
			
		managerref.push(&managers[i]);
		}
		

	}
	if (managerref.length != 0)
	{


		std::sort(&managerref[0], &managerref[0] + managerref.length, [](component_table* a, component_table* b) {
			return a->priority > b->priority;
			});
	}
	for (component_table* mgr : managerref) {
		for (Archtype* archtype : arch.archtypes) {
			
			if (!archtype->has_component(mgr->id)) continue;

			// 1) Snapshot the count _once_
			size_t originalCount = archtype->elems.length;

			// 2) Index by integer, not range-for
			for (size_t i = 0; i < originalCount; ++i) {
				obj   o = archtype->elems[i];           // read by value
				auto* c = (component*)mgr->store[o.Id.id];
				c->update();  
				
			}
		}
	}
	managerref.destroy();
	
}



obj gameobject::Ecs::CreateEntity(v3::Vec3 SpawnPos)
{
	obj object = obj();
	InitObj(object);
	object.addcomponent<transform_comp>()->transform.position = SpawnPos;
	return object;

	arch.check();
}




gameobject::component_table::component_table()
{
	
	id = comp::None;
	priority = -1;
	utype = updatenone;
	
}

void gameobject::component_table::create(comp::Id mid, size_t bytesize, size_t alignment)
{
	id = mid;
	pool = dynPool::flux<component>(bytesize,alignment);
}

void gameobject::component_table::init(component* sample)
{
	
	priority = sample->priority;
	utype = sample->utype;
}


constexpr size_t gameobject::ComponentMapper::operator()(component* comp) const noexcept
{
	return comp->owner.Id.id;
}
