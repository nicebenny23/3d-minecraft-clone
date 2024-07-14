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
const unsigned int SCR_WIDTH = 4000;
const unsigned int SCR_HEIGHT = 3000;

int main()
{
    timename::inittime();
    randominit();
    window::createcurwindow(1600,1200);
    userinput::initiate();
    gameobject::initmap();
    gameobject::initobjs();
 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
  
    
  // createdirectory("worldstorage");
    camera::initilize();

    renderer::load();
   grid::initgrid();
  gridutil::computeallcover();
  gridutil::redolighting();
    Vector3 spawnpos = glm::vec3(-100, 17, 200);
  
    uirender::initrenderlist();
   uirender::newbox("images\\crosshair.png", v2::unitv/32, v2::zerov);
    userinput::endupdate();
    vobj::vbuf testbuf;
    vobj::vao testvoa;
    testbuf.generate(GL_ARRAY_BUFFER);
    testvoa.generate();
    texture mtex = texture("images\\stone.png", png);
    meshname::mesh newmehs = *meshname::loadmesh("cubetest.obj", mtex,spawnpos);
    aabb::initcolrect();
    aabb::colrect pos = aabb::colrect(spawnpos, Vector3(.5,1,.5), false);
entity::entityref human = entity::createentity(spawnpos, "");
float lastupdate = 0;
human.toent()->addcomponent<playermovement>();
human.toent()->addcomponent<playermod>();
while (!window::shouldclose())
    {
   timename::calcfps();
    entity::entity* a = human.toent();
   
      
        
        window::processInput();
      
        renderer::clear();
        gameobject::runupdateloop();
        pos.center = human.toent()->pos;

        collision::update();
        human.toent()->pos = pos.center.glm();
         

     
         camera::calculateyawandpitch();
        
         camera::setcamerapos(v3::Vector3(0,0,0)+human.toent()->pos);
         camera::sendoffviewmatrix();
        
       
      

        userinput::endupdate();
        // update shader uniform
        // bind texture
        float timeValue = glfwGetTime();
     
        grid::reupdatechunkborders();
        grid::load();
        if (grid::gridchanged())
        {

           gridutil::computeallcover();
           gridutil::redolighting();
        }
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
         blockrender::initdatabuffer();
meshname::rendermesh(&newmehs, testvoa, testbuf);
uirender::renderuilist();
      
        gameobject::deleteobjs();
      
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
