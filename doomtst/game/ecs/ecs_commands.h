#include "component.h"
#pragma once
namespace ecs {


	struct DestroyEntity{
		entity ent;
	};
	struct DestroyComponent {
		component_id id;
		entity owning_entity;
		DestroyComponent(entity ent, component_id comp_id) :owning_entity(ent), id(comp_id) {
		}
	};


}