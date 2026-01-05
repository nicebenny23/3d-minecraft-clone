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
	int x = 42;

	// reference insertion
	insertion<int&> ref_ins(x, true);

	ref_ins.on_insert([](int& p) {
		std::cout << "Before: " << p << "\n";
		p = 100;
		std::cout << "After: " << p << "\n";
		});

	std::cout << "Outside: " << ref_ins.value << "\n";


	int m = x;
    SetCurrentDirectory(L"C:\\Users\\User\\source\\repos\\nicebenny23\\3d-minecraft-clone\\doomtst");
    rungame();
}