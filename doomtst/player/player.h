#include "../game/entity.h"
#include "../game/collision.h"
#include "../renderer/uirender.h"
#include "playermovment.h"
#include "../playermodification.h"
#include "../game/entitystate.h"
#include"playerhealth.h"
#ifndef player_HPP
#define player_HPP
namespace player {
	extern entityname::entityref goblin;
	void initplayer();
	extern bool playeractive;
}

#endif // !player_H