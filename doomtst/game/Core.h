#pragma once
#include "../renderer/Window.h"
#include "../world/grid.h"
#include "../util/userinput.h"
#include "GameContext.h"
#include "entity.h"
#include "../util/time.h"
#include "../renderer/guirender.h"
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
      
       CtxName::Context* ctx;

        timename::TimeManager Time;
        userinput::InputManager Inp;
        window::Window Window;


        grid::Grid Grid;

        gameobject::OCManager OC;
    };
    extern Engine game;
    
}


