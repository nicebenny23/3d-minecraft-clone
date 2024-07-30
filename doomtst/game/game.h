#include "../game/camera.h"
#include "../renderer/mesh.h"
#include "../renderer/shader.h"
#include <iostream>
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
#include "../game/particles.h"
#include "../renderer/blockrender.h"
#include "../world/managegrid.h"
#include "../util/time.h"
#include "../game/collision.h"
#include "../renderer/uirender.h"
#include "../player/playermodification.h"
#include "../util/fileloader.h"
#include "../renderer/textrender.h"
#include "../player/player.h"
#include "../entities/onhit.h"
#include "../entities/slime.h"
#include "../game/navigation.h"
#include "../entities/entityspawner.h"
// settings
#include "../renderer/grandrenderer.h"

#include "../world/noise.h"

#ifndef game_HPP
#define game_HPP
void endupdate() {
    entityname::deleteobjs();

    userinput::endupdate();

    window::swapbuffer();
    glfwPollEvents();
    renderer::clear();

}
void preupdate() {


    camera::setcamerapos(player::goblin.toent()->transform.position);

    timename::calcfps();

    window::processInput();


}

void updateobjs() {


    collision::update();

    grid::runblockupdates();
    entityname::runupdateloop();

    collision::sendplayercameraray();
    camera::cameraupdate();


}
void update() {

    preupdate();
    updateobjs();
    spawn();
    gridutil::gridupdate();
    rendergame();
    endupdate();
}
void init() {

    std::string o1 = std::string("C:/Users/bchar/source/repos/doomtst/doomtst/worldstorage");

    std::string o2 = std::string("C:/Users/User/source/repos/nicebenny23/3d-minecraft-clone/doomtst/worldstorage");
    deleteFilesInFolder(o1);

    timename::inittime();
    randominit();
    window::createcurwindow(1600, 1200);
    userinput::initiate();
    gameobject::initmap();
    entityname::initobjs();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Assert("Failed to initialize GLAD");

    }

    aabb::initCollider();

    inittextarray();
    uirender::initrenderlist();
    player::initplayer();
    initrandomdirs();
    renderer::load();
    blockrender::initblockrendering();
    camera::initilize();





  
    grid::initgrid();
    gridutil::computeallcover();
    gridutil::redolighting();
    uirender::newbox("images\\crosshair.png", v2::unitv / 32, v2::zerov, -3);
    userinput::endupdate();

    glfwSwapInterval(0);
}
void render() {




}
void rungame()
{

    init();
   


    float lastupdate = 0;
    while (!window::shouldclose())
    {
        update();
        lastupdate += timename::dt;
        if (lastupdate > 1)
        {

            lastupdate = 0;
            // Vector3 pos1 = human.toent()->pos;
            std::cout << 1 / timename::dt << '\n';
        }
    }

    glfwTerminate();
    return;
}

#endif