
#include "../game/ecs/game_object.h"

#include "../util/dynamicarray.h"
#include "../items/Container.h"
#include "playerinventory.h"
#include "../player/playerhealth.h"
#pragma once 
namespace player {
	struct player_eat_behavior : ecs::component {
		player_eat_behavior(timing::WorldClock& clock, ecs::Constrained<ui::UiImage> img) :duration(clock),food(img){

		}
		ecs::Constrained<ui::UiImage> food;
		timing::Duration duration;
	};

	struct PlayerEater :ecs::System {
		void run(ecs::Ecs& world) {
			if (world.get_resource<ui::MenuState>().menu_open()) {
				return;
			}
				ecs::View < Health::EntityHealth, player::inventory, player_eat_behavior> eat_query(world);
				for (auto&& [health, inventory, eater] : eat_query) {
					stn::ScopeGuard guard([&]() {eater.duration.set(.5f);	});

					eater.food.get_component<ui::UiEnabled>().disable();
					if (!world.get_resource<userinput::InputManager>().left_mouse().held) {
						continue;
					}
					stn::Option<items::item_stack&> stack = inventory.selected();
					if (!stack) {
						continue;
					}

					items::item_id id = stack.unwrap().contained_id();
					const items::item_type& type = world.get_resource<items::ItemTypes>().from_id(id);
					size_t food = type.traits(world).food;
					if (!food) {
						continue;
					}

					eater.food.get<ui::UiImage>().set_image(type.traits(world).image_path);
					size_t health_after = stn::min(health.max_health, health.current_health + food);
					if (health.current_health == health_after) {
						continue;
					}
					if (!stack.unwrap().can_remove(1)) {
							continue;
					}

					double progress = eater.duration.remaining().unwrap_or(0)*2;
					
					eater.food.get_component<ui::UiBounds>().local.scale = v2::Vec2(progress, progress)*.1 + v2::Vec2(.3, .3);
					eater.food.get_component<ui::UiImage>().current_color.a = progress;
					eater.food.get_component<ui::UiEnabled>().enable();
					if (eater.duration.is_inactive()) {
							stack.unwrap().remove(1);
						health.current_health = health_after;
					}
					else {
						guard.disarm();
					}
				}
							
		}
		
	
	};
}

// ! playerplace_H
