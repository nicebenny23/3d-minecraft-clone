#pragma once 

#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../debugger/console.h"
#include "player_look.h"

struct playerattackcomp : ecs::component {
	void wearduribilty() {
		stn::Option<items::item_stack&> select = owner().get_component<player::inventory>().selected();
		if (select) {
			if (select.unwrap().owner().has_component<items::item_durability>()) {
				select.unwrap().owner().get_component<items::item_durability>().use();
			}
		}
	}
	size_t computeattackdmg() {
		return 1;
	}

	void update() {


		math::ray cameraray = math::ray(camera::campos(), camera::campos() + camera::GetCamFront() * 7);

		voxtra::WorldRayCollision Hit = owner().get_component<player::PlayerCursor>().Hit;
		if (!Hit) {
			return;
		}
		if (!world().get_resource<userinput::InputManager>().left_mouse().pressed) {
			return;
		}
		voxtra::RayWorldHit closest = Hit.unwrap();
		if (!closest.collider.owner().has_component<Health::EntityHealth>()) {
			return;
		}

		size_t damage = computeattackdmg();
		if (closest.owner().has_component<physics::rigidbody>()) {
			world().write_command(Health::AttackCommand{.damage= damage,.center = cameraray.start,.body = closest.owner()});

		}
		wearduribilty();


	}

};



// !ecs_HPP
