#include "../game/camera.h"
#include "../renderer/mesh.h"
#include "../renderer/shader.h"
#include <iostream>
#include "../sound/sound.h"
#include <cmath>
#include "../renderer/texture.h"
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
    entityname::deleteobjs();

    userinput::endupdate();
    updateltick();
    window::swapBuffers();
    glfwPollEvents();
    renderer::clearscreen();

}
void startframe() {


    timename::calcfps();
     tick::trytick();
   managemenus();

}

void updateworld() {

    
    
    collision::update();
    gameobject::updatecomponents();

   
    gridutil::gridupdate();
    camera::cameraupdate();

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
    
    deleteolddata();

    window::createWindow();
    userinput::initiate();


    timename::inittime();
    initrandom();
    initrandomdirs();
  
    
    gameobject::initmap();
    entityname::initobjs();

    gameobject::initmanagerlist();
  

    aabb::initCollider();

    ui::createuilist();
    inittextarray();
  
    player::initplayer();

    renderer::load();
    blockrender::initblockrendering();
    
    camera::initilize();
    ui::createuielement<uibox>("images\\crosshair.png", v2::unitv / 32, v2::zerov, -3);

    grid::initgrid();
    gridutil::computeallcover();
    gridutil::redolighting();
    userinput::endupdate();

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
    while (!window::shouldClose())
    {
        
        update();
        lastupdate += timename::dt;
        if (lastupdate > 1)
        {

            lastupdate = 0;
         
         warn(timename::fps);
        }
    }
    endgame();
    return;
}

#endif