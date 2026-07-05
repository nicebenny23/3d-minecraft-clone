#pragma once

#include "../game/ecs/filtered_object.h"
// settings

namespace player {
	struct PlayerTag : ecs::component{



	};
	struct PlayerResource :ecs::resource {
		ecs::Constrained<PlayerTag> player;
		ecs::obj get() {
			return player.object();
		}
		PlayerResource(ecs::obj player_obj) :player(player_obj) {

		}
	};
	inline ecs::obj player_for(ecs::Ecs& world) {
		return world.get_resource<PlayerResource>().get();
	}
	void initplayer(ecs::obj& player);
}
