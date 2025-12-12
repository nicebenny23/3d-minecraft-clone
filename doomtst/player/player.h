#pragma once
#include "../game/entity.h"
#include "../game/entitystate.h"

// settings

namespace player {
	struct player_tag : ecs::component{



	};
	extern ecs::obj goblin;
	void initplayer();
	extern bool playeractive;

}
