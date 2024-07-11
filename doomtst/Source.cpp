#include "game/camera.h"

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
#include "playermovment.h"
#include "uirender.h"
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
  
    
    
    camera::initilize();

    renderer::load();
   grid::initgrid();
  gridutil::computeallcover();
  gridutil::redolighting();
    glm::vec3 cam = glm::vec3(-100, 17, 200);
  
    uirender::initrenderlist();
   // uirender::uibox("crosshair.png", v2::unitv*333, v2::zerov);
    userinput::endupdate();
    
    aabb::initcolrect();
    aabb::colrect pos = aabb::colrect(cam, Vector3(.5,1,.5), false);
entity::entityref human = entity::createentity(v3::Vector3(0, 18, 0), "");
float lastupdate = 0;
human.toent()->addcomponent<playermovement>();
while (!window::shouldclose())
    {
   timename::calcfps();
    entity::entity* a = human.toent();
   
      
        
        window::processInput();
      
        glClearColor(0,0,0, 0.0f);
      
       glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
   
        gameobject::runupdateloop();
        pos.center = human.toent()->pos;

        collision::update();
        human.toent()->pos = pos.center.glm();
         

     
         camera::calculateyawandpitch();
         collision::collidecamray();
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
     
            blockrender::initdatabuffer();
        //   uirender::renderuilist();
      
        gameobject::deleteobjs();
      
    window::swapbuffer();
        glfwPollEvents();
        lastupdate += timename::dt;
        if (lastupdate > 1)
        {

            lastupdate = 0;
           
            std::cout << (1/timename::dt) << '\n';
        }
    }
   
    glfwTerminate();
    return 0;
}
