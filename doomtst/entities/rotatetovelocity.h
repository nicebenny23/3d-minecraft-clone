#include "../game/objecthelper.h"
#include "../game/rigidbody.h"
#pragma once 
struct rotatetwordsvel: ecs::component
{
	void update() {

		Vec3 vel = owner().get_component<rigidbody>().velocity;
		vel= normal(vel);
		owner().get_component<ecs::transform_comp>().transform.OrientDir(vel);
	}
};


// !
