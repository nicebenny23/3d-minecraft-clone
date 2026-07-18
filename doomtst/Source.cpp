#pragma once
#include "game/game.h"
#include <filesystem>
int main(int argc, char** argv) {
	std::filesystem::current_path("resources");
    rungame();
}