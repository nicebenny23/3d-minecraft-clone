
#include "../game/rigidbody.h"
#pragma once 
struct rotate_to_velocity: ecs::component
{
	void update() {

		Vec3 vel = owner().get_component<physics::rigidbody>().velocity;
		vel= normal(vel);
		owner().get_component<ecs::world_transform>().transform.look_towards(vel);
	}
};


// !
