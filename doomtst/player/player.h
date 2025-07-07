#pragma once
#include "../game/entity.h"
#include "../game/entitystate.h"

// settings

namespace player {
	struct player_tag : gameobject::component {



	};
	extern gameobject::obj goblin;
	void initplayer();
	extern bool playeractive;

}
