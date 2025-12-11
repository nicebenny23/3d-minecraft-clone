#include "component.h"
#pragma once
namespace ecs {


	struct DestroyEntity{
		space_id ent;
	};
	struct DestroyComponent {
		component_id id;
		space_id owning_entity;
		DestroyComponent(space_id ent, component_id comp_id) :owning_entity(ent), id(comp_id) {
		}
	};


}