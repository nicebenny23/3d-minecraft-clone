#pragma once

#include "../game/ecs/filtered_object.h"
// settings

namespace player {
	struct player_tag : ecs::component{



	};
	extern ecs::obj goblin;
	struct player_resource :ecs::resource {
		ecs::Constrained<player_tag> player;
		ecs::obj get() {
			return player.object();
		}
		player_resource(ecs::obj player_obj) :player(player_obj) {

		}
	};
	inline ecs::obj player_for(ecs::Ecs& world) {
		return world.get_resource<player_resource>().get();
	}
	void initplayer(ecs::obj& player);
	extern bool playeractive;

}
