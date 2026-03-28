#pragma once
#include "../util/dynamicarray.h"
#include "../renderer/uibox.h"
#include "../game/ecs/game_object.h"
#include <conio.h>
#include "../items/block_definitions.h"
#include "../game/entityutil.h"
#include "playerinventory.h"
#include "../util/cached.h"

struct playerhealth: ecs::component
{
	
	stn::Option<ui::ui_image> damage_decal;
	
	bool dmgimmune = false;
	void killplayer() {
		world().write_command<>(Core::CloseGameCommand());
	
	}

	array<ui::ui_image>healthboxes;
	void start(){
		damage_decal= std::move(ui::ui_image(world(),"images\\red_back.png", "on_dmg_texture", math::Box2d::origin_centered(v2::zerov), -3));
		size_t max_health = owner().get_component<Health::EntityHealth>().max_health;
		v2::Vec2 scale = v2::unitv / 100;
		healthboxes = array<ui::ui_image>();
		for (int i = 0; i < max_health; i++) {
			v2::Vec2 pos = v2::Vec2(i / 40.f-.4f, -.45);

			healthboxes.push(ui::ui_image(world(), "images\\health.png", "HealthTexture", math::Box2d(pos, scale), 55));
		}
	}
	
	void update() {
		Health::EntityHealth& health = owner().get_component<Health::EntityHealth>();
			size_t max_health = health.max_health;
		
			for (int i = 0; i < health.current_health; i++) {

				healthboxes[i].enable();
			}
			for (int i = health.current_health; i < max_health; i++) {

				healthboxes[i].disable();
			}
	
	}
};

namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<playerhealth> = {
		.priority = -233,
	};
}