#pragma once
#include <vector>
#include "game/game.h"
#include "game/entity.h"
#include "game/ecs/game_object.h"
#include "game/ecs/query.h"
#include "game/ecs/commands.h"
#include <filesystem>

struct mean :ecs::component {
    mean() {

    }
    void update() {
        l = 2;
        
    }
    int l = 1;
};
int main(int argc, char** argv) {
    static_assert(std::constructible_from<ecs::component>);
    ecs::Ecs world= std::move(ecs::Ecs(10));
    ecs::obj obj = world.spawn_empty();
    obj.add_component<mean>();
    world.run_systems();
    obj.destroy_component<mean>();
    world.run_commands();
    SetCurrentDirectory(L"C:\\Users\\User\\source\\repos\\nicebenny23\\3d-minecraft-clone\\doomtst");
    rungame();
    // Now all 1relative paths (shaders/, textures/, dll loads) work *no matter how* you launch the EXE
    // … your existing init + render loop …

}