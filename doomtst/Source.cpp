#include "game/camera.h"
#include "mesh.h"
#include "renderer/shader.h"
#include <iostream>
#include <cmath>
#include "renderer/texture.h"
#include "renderer/Window.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "util/userinput.h"
#include "renderer/renderer.h"
#include "world/chunk.h"
#include "game/objecthelper.h"
#include "world/grid.h"
#include "game/collision.h"
#include "renderer/blockrender.h"
#include "world/managegrid.h"
#include "util/time.h"
#include "game/playermovment.h"
#include "renderer/uirender.h"
#include "playermodification.h"
#include "util/fileloader.h"
// settings
const Vector3 spawnpos = glm::vec3(-100, -30, 200);
const unsigned int SCR_WIDTH = 4000;
const unsigned int SCR_HEIGHT = 3000;
void init() {
    timename::inittime();
    randominit();
    window::createcurwindow(1600, 1200);
    userinput::initiate();
    gameobject::initmap();
    entityname::initobjs();

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        Assert( "Failed to initialize GLAD");
      
    }


    camera::initilize();

    renderer::load();
    grid::initgrid();
    gridutil::computeallcover();
    gridutil::redolighting();


    uirender::initrenderlist();
    uirender::newbox("images\\crosshair.png", v2::unitv / 32, v2::zerov);
    userinput::endupdate();
    aabb::initcolrect();
}

int main()
{
    init();
    vobj::vbuf testbuf;
    vobj::vao testvoa;
    testbuf.generate(GL_ARRAY_BUFFER);
    testvoa.generate();
    texture mtex = texture("images\\crystalore.png", png);
    meshname::mesh newmehs = *meshname::loadmesh("newtest.obj", mtex,spawnpos);
  
  
    entityname::entityref human = entityname::createentity(spawnpos, "");
human.toent()->addcomponent<colrect>(spawnpos,unitv/2,false);

float lastupdate = 0;
human.toent()->addcomponent<playermovement>();
human.toent()->addcomponent<playermod>();
while (!window::shouldclose())
    {
    timename::calcfps();

    window::processInput();

    renderer::clear();
    entityname::runupdateloop();

    collision::update();



    camera::calculateyawandpitch();

    camera::setcamerapos(human.toent()->pos);
    camera::sendoffviewmatrix();

    userinput::endupdate();
    // update shader uniform
   
    grid::reupdatechunkborders();
    grid::load();
    if (grid::gridchanged())
    {
        gridutil::computeallcover();
        gridutil::redolighting();
    }

    meshname::rendermesh(&newmehs, testvoa, testbuf);
    blockrender::initdatabuffer();
   
    uirender::renderuilist();

    entityname::deleteobjs();

    window::swapbuffer();
    glfwPollEvents();
    lastupdate += timename::dt;
        if (lastupdate > 1)
        {

            lastupdate = 0;
            Vector3 pos1 = human.toent()->pos;
            std::cout <<1/timename::dt << '\n';
        }
    }
   
    glfwTerminate();
    return 0;
}
