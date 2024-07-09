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
// settings
const unsigned int SCR_WIDTH = 4000;
const unsigned int SCR_HEIGHT = 3000;

int main()
{
  
    randominit();
    window::createcurwindow(1600,1200);
    userinput::initiate();
    gameobject::initmap();
    gameobject::initobjs();
    block* blk = new block(Vector3(1, 3, 4), 4);
   
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
  
    glEnable(GL_DEPTH_TEST);
    
    camera::initilize();

    renderer::load();
   grid::initgrid();
    glm::vec3 cam = glm::vec3(-100, 17, 200);
  
  
    userinput::endupdate();
    float time = 0;
    float dt = 0;
    aabb::initcolrect();
    aabb::colrect pos = aabb::colrect(cam, Vector3(.5,1,.5), false);
entity::entityref human = entity::createentity(v3::Vector3(70, 17, 0), "");
float lastupdate = 0;
while (!window::shouldclose())
    {
    entity::entity* a = human.toent();
    dt = glfwGetTime() - time;
        time = glfwGetTime();
        float speed = 10;
        if (userinput::getinputkey('s').held)
        {
            human.toent()->pos -= v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * (dt * speed);
        }
        if (userinput::getinputkey('w').held)
        {
            human.toent()->pos += v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * (dt * speed);
        }
        if (userinput::getinputkey('a').held)
        {
            human.toent()->pos += v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw)) ) * (dt * speed);
        }
        if (userinput::getinputkey('d').held)
        {
            human.toent()->pos -= v3::Vector3(sin(glm::radians(camera::yaw)) , 0, -cos(glm::radians(camera::yaw)))*(dt*speed);
        }
        if (userinput::getinputkey(' ').held)
        {
            human.toent()->pos += v3::Vector3(0,1,0) * (dt * speed);
        }
        if (userinput::getinputkey('z').held)
        {
            human.toent()->pos -= v3::Vector3(0,1, 0) * (dt * speed);
        }
        pos.center = human.toent()->pos;
     
        collision::update();
        human.toent()->pos = pos.center.glm();

        
        window::processInput();
      
        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     
   
        glm::mat4 trans = glm::mat4(1.0f);
        
       
      

     
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
      
            blockrender::initdatabuffer();
        
      
        gameobject::deleteobjs();
      
    window::swapbuffer();
        glfwPollEvents();
        lastupdate += dt;
        if (lastupdate > 1)
        {

            lastupdate = 0;
           
            std::cout << (1/dt) << '\n';
        }
    }
   
    glfwTerminate();
    return 0;
}
