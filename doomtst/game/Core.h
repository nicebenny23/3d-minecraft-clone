#pragma once
#include "../renderer/Window.h"
#include "../world/grid.h"
#include "../util/userinput.h"
#include "GameContext.h"
#include "entity.h"
#include "../renderer/renderer.h"
#include "System.h"
#include "../util/time.h"
#include "../renderer/guirender.h"
#include "../world/world.h"
//start of the application
namespace Core {
    struct Engine
    {

        void ConnectToContext();
        void createWindow();
        void CreateGrid();
        void InitTime();
        void InitInput();
        void InitOC();
        void CreateWorld();
        void InitRenderer();
       CtxName::Context* ctx;
        timename::TimeManager Time;
        userinput::InputManager Inp;
        window::Window Window;
        renderer::Renderer ren;
        World::world world;
        grid::Grid Grid;
		Engine():OC(2<<20) {

		}
        ecs::Ecs OC;
    };
    extern Engine game;
    
}


