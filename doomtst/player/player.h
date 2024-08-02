#include "../game/entity.h"
#include "../game/collision.h"
#include "../renderer/uirender.h"
#include "playermovment.h"
#include "playermodification.h"
#include "../game/entitystate.h"
#include"playerhealth.h"
#include "playertpsword.h"
// settings
#ifndef player_HPP
#define player_HPP
namespace player {
	extern entityname::entityref goblin;
	void initplayer();
	extern bool playeractive;
	void calculateyawandpitch();

}

#endif // !player_H
