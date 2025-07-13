#include "../game/camera.h"
#include "../renderer/ModelMesh.h"
#include "../renderer/shader.h"
#include <iostream>
#include <cmath>
#include "../renderer/renderer.h"
#include "../renderer/Window.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../util/userinput.h"
#include "../world/chunk.h"
#include "../game/objecthelper.h"
#include "../world/grid.h"
#include "../renderer/blockrender.h"
#include "../world/managegrid.h"
#include "../util/time.h"
#include "../game/collision.h"
#include "../util/fileloader.h"
#include "../player/player.h"
#include "../game/navigation.h"
#include "../renderer/grandrenderer.h"
#include "../world/noise.h"
#include "../world/tick.h"
#include "../block/liquid.h"
#include "../debugger/console.h"
#include "../items/menu.h"
#include "Core.h"
#include "rigidbody.h"
#include "../player/playermovment.h"
#include "../game/System.h"
#pragma once 
struct IAmAPrinter:System {
    void run(gameobject::Ecs* ecs) override {
        
    }
};
void endframe() {
    CtxName::ctx.Inp->endupdate();
    updateltick();
    CtxName::ctx.Window->SwapBuffers();
    glfwPollEvents();
    CtxName::ctx.Ren->Clear();

 
    CtxName::ctx.OC->commands.pop();
}
void startframe() {


    CtxName::ctx.Time->calcfps();
     tick::trytick();
   managemenus();

}

void updateworld() {

    
    collision::update();
    CtxName::ctx.Executor->Run();

CtxName::ctx.OC->updatecomponents(gameobject::Framecall);
gridutil::gridupdate();

}
void update() {

    startframe();
    updateworld();
    rendergame();
    endframe();
}
void deleteolddata() {
    std::string o1 = std::string("C:/Users/bchar/source/repos/doomtst/doomtst/worldstorage");

    std::string o2 = std::string("C:/Users/User/source/repos/nicebenny23/3d-minecraft-clone/doomtst/worldstorage");
    deleteFilesInFolder(o1);

}
void init() {
    initrandom();

    Core::game.ConnectToContext();
    deleteolddata();
    Core::game.createWindow();
    Core::game.InitRenderer();
    Core::game.InitOC();
    aabb::initCollider();
    ui::createuilist();
    inittextarray();
    player::initplayer();
    blockrender::initblockrendering();
    ui::createuielement<uibox>("images\\crosshair.png", "CrosshairTexture", v2::unitv / 32, v2::zerov, -3);
    Core::game.CreateGrid();
    gridutil::computeallcover();
    gridutil::redolighting();
    CtxName::ctx.Inp->endupdate();

    glfwSwapInterval(0);
}

void endgame() {

    guirender::destroygui();
    glfwTerminate();

}
void rungame()
{
    init();
     CtxName::ctx.Executor->push(new IAmAPrinter());
     CtxName::ctx.Executor->push(new RigidbodySystem());

     CtxName::ctx.Executor->push(new PlayerMovementSys());
     float lastupdate = 0;
    while (!CtxName::ctx.Window->shouldClose())
    {
        update();
    }
    endgame();
    return ;
}

