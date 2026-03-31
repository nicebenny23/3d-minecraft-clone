#pragma once 

#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/health.h"
#include "../debugger/console.h"
#include "player_look.h"
namespace player {
	struct PlayerAttack : ecs::component {
	};
	struct PlayerAttacker :ecs::System {
		void run(ecs::Ecs& world) {
			if (!world.get_resource<userinput::InputManager>().left_mouse().pressed) {
				return;
			}
			ecs::View < PlayerAttack, player::PlayerCursor,player::inventory> attack_query(world);
			for (auto&&[attack,cursor,inventory]:attack_query) {

				if (cursor.Hit) {
					voxtra::RayWorldHit closest = cursor.Hit.unwrap();
					ecs::obj object = closest.collider.object();
					if (object.has_components<Health::EntityHealth, physics::rigidbody>()) {
						stn::Option<ecs::obj> item = inventory.selected_object();
						if (object.get_component<Health::EntityHealth>().damage_delay_timer.is_inactive()) {
							size_t dmg = 1;

							if (item) {
								items::item_id id = item.unwrap().get_component<items::item_stack>().contained_id();
								dmg = world.get_resource<items::item_types>().from_id(id)
									.tool.member(&items::tool_traits::damage).unwrap_or_default();
							}
							world.write_command(Health::AttackCommand{ .knockback_multiplier = 1,.damage = dmg,.center = closest.ray().start,.body = object});
							item.unwrap().get_component_opt<items::item_durability>().then([](items::item_durability& dur) {dur.use(); });
						}
						
					}
				}
			}
			
		}

	};
}