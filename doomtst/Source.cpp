#pragma once
#include <vector>
#include "game/game.h"
#include "game/entity.h"
#include "game/ecs/game_object.h"
#include "game/ecs/query.h"
#include "game/ecs/commands.h"
#include "game/ecs/resources.h"
#include "game/ecs/message.h"
#include <filesystem>
int main(int argc, char** argv) {
	
    SetCurrentDirectory(L"C:\\Users\\User\\source\\repos\\nicebenny23\\3d-minecraft-clone\\doomtst");
    rungame();
}