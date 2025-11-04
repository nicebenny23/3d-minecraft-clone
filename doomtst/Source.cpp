#pragma once
#include <vector>
#include "game/game.h"
#include "game/entity.h"
#include <filesystem>
int main(int argc, char** argv) {
    SetCurrentDirectory(L"C:\\Users\\User\\source\\repos\\nicebenny23\\3d-minecraft-clone\\doomtst");
    rungame();
    // Now all 1relative paths (shaders/, textures/, dll loads) work *no matter how* you launch the EXE
    // … your existing init + render loop …

}