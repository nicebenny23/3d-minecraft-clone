

#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../game/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "../game/close.h"

struct playermovement : ecs::component {
	timing::Duration last_grounded;
	timing::Duration jump_buffer;
	timing::Duration held;
	playermovement(timing::WorldClock& clock):jump_buffer(clock),last_grounded(clock),held(clock){
		
	}
};

struct PlayerMovementSys : ecs::System {
	const float coyote_duration = 0.15f;
	const float buffer_duration = 0.10f;
	const float jump_strength = 150.0f * 12.0f / 200.0f;
	double hold_dur = .175f;
	virtual void run(ecs::Ecs& ecs) {
		if (!player::in_game(ecs)) {
			return;
		}

		auto view = ecs::View<physics::RigidBody, playermovement, core::LocalTransform, physics::Buoyancy>(ecs);

		for (auto [body, movement, transform, buoyancy] : view) {
			userinput::InputManager& man = ecs.get_resource<userinput::InputManager>();
			float dt = ecs.ensure_resource<timing::WorldClock>().dt;
			double now = ecs.ensure_resource<timing::WorldClock>().now();
			float speed =32;
			float effSpeed = dt * speed;
			Vec3 forward = transform.transform.normal_dir().with_y(0).normal();
			Vec3 right = transform.transform.right_dir().with_y(0).normal();
			
			if (man.key('w').held) body.velocity += forward * effSpeed;
			if (man.key('s').held) body.velocity -= forward * effSpeed;
			if (man.key('d').held) body.velocity += right * effSpeed;
			if (man.key('a').held) body.velocity -= right * effSpeed;

			if (body.on_ground) {

				movement.last_grounded.set(coyote_duration);
			}

			if (man.key(' ').pressed) {

				if (movement.jump_buffer.remaining_or_default()<=.1f) {

					movement.jump_buffer.set(buffer_duration);
				}
			}
			if (movement.held.is_active()&& !man.key(' ').held) {
				body.add_force(physics::Force(v3::Vec3(0, -7/hold_dur, 0) * body.mass));
			}
			if (buoyancy.in_water) {
				double float_amt = 35;
				if (man.key(' ').held) {
					body.velocity.y += float_amt * dt;
				}
			}
			else {
				
				if (movement.jump_buffer.is_active() && movement.last_grounded.is_active()) {
					body.velocity.y = jump_strength;
					movement.last_grounded.set(0.0);
					movement.held.set(hold_dur);
				}


				if (man.key('f').held) {
					transform.transform.position += transform.transform.normal_dir() * 50 * dt;
				}
				if (man.key('h').held) {
					transform.transform.position += transform.transform.normal_dir() * 10 * dt;
				}


			}
		}
	}
};
