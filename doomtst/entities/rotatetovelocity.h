
#include "../game/rigidbody.h"
#pragma once 
struct rotate_to_velocity: ecs::component
{
	void update() {

		Vec3 vel = owner().get_component<physics::RigidBody>().velocity;
		vel= normal(vel);
		owner().get_component<core::LocalTransform>().transform.look_towards(vel);
	}
};


// !
