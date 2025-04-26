#include "../game/camera.h"
#include "../renderer/ModelMesh.h"
#include "../renderer/shader.h"
#include <iostream>
#include "../sound/sound.h"
#include <cmath>
#include "../renderer/renderer.h"
#include "../renderer/Window.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../util/userinput.h"
#include "../renderer/renderer.h"
#include "../world/chunk.h"
#include "../game/objecthelper.h"
#include "../world/grid.h"

//#include "../game/particles.h"
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
#ifndef game_HPP
#define game_HPP
void endframe() {
    CtxName::ctx.EntMan->DeleteObjs();

    CtxName::ctx.Inp->endupdate();
    updateltick();
    CtxName::ctx.Window->SwapBuffers();
    glfwPollEvents();
    renderer::Ren.Clear();

}
void startframe() {


    CtxName::ctx.Time->calcfps();
     tick::trytick();
   managemenus();

}

void updateworld() {

    
    
    collision::update();
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
    Core::game.InitOC();
    Core::game.createWindow();
    Core::game.InitEntMan();
    aabb::initCollider();
    ui::createuilist();
    inittextarray();
    player::initplayer();
    renderer::load();
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
    
    float lastupdate = 0;
    while (!CtxName::ctx.Window->shouldClose())
    {
        
        update();
        lastupdate += CtxName::ctx.Time->dt;
        if (lastupdate > 1)
        {

            lastupdate = 0;
         
         warn(CtxName::ctx.Time->fps);
        }
    }
    endgame();
    return;
}

#endif