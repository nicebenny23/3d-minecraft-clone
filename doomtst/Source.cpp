#include <glad/glad.h>
#include "camera.h"
#include "shader.h"
#include <iostream>
#include <cmath>
#include "texture.h"
#include "Window.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "userinput.h"
#include "renderer.h"
#include "chunk.h"
#include "objecthelper.h"
#include "grid.h"
#include "collision.h"
#include "blockrender.h"
// settings
const unsigned int SCR_WIDTH = 4000;
const unsigned int SCR_HEIGHT = 3000;

int main()
{
   
    // glfw: initialize and configure
    // ------------------------------
    randominit();
    window::createcurwindow(1600,1200);
    userinput::initiate();
    gameobject::initmap();
    gameobject::initobjs();
   
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
  
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    glEnable(GL_DEPTH_TEST);
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //all vertex data including tex and color
    camera::initilize();

    renderer::load();
    grid::initgrid();
    glm::vec3 cam = glm::vec3(-100, 17, 200);
    // render loop
    // -----------
    
    userinput::endupdate();
    float time = 0;
    float dt = 0;
    aabb::initcolrect();
    aabb::colrect pos = aabb::colrect(cam, Vector3(.5,1,.5), false);
entity::entityref human = entity::createentity(v3::Vector3(0, 17, 70), "");
float lastupdate = 0;
while (!window::shouldclose())
    {
    entity::entity* a = human.toobj();
    dt = glfwGetTime() - time;
        time = glfwGetTime();
        float speed = 10;
        if (userinput::getinputkey('s').held)
        {
            human.toobj()->pos -= v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * (dt * speed);
        }
        if (userinput::getinputkey('w').held)
        {
            human.toobj()->pos += v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * (dt * speed);
        }
        if (userinput::getinputkey('a').held)
        {
            human.toobj()->pos += v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw)) ) * (dt * speed);
        }
        if (userinput::getinputkey('d').held)
        {
            human.toobj()->pos -= v3::Vector3(sin(glm::radians(camera::yaw)) , 0, -cos(glm::radians(camera::yaw)))*(dt*speed);
        }
        if (userinput::getinputkey(' ').held)
        {
            human.toobj()->pos += v3::Vector3(0,1,0) * (dt * speed);
        }
        if (userinput::getinputkey('z').held)
        {
            human.toobj()->pos -= v3::Vector3(0,1, 0) * (dt * speed);
        }
        pos.center = human.toobj()->pos;
        //addindependent move on each axis
        collision::update();
        human.toobj()->pos = pos.center.glm();

        // input
        // -----
        window::processInput();
      
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 0.0f);
     
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // be sure to activate the shader before any calls to glUniform
      
   
        glm::mat4 trans = glm::mat4(1.0f);
        
       
      

         float radius =userinput::mousepos.x;
         camera::calculateyawandpitch();
         collision::collidecamray();
         camera::setcamerapos((human.toobj()->pos.glm()));
         camera::sendoffviewmatrix();
        
       
       
// note that we're translating the scene in the reverse direction of where we want to move

        userinput::endupdate();
        // update shader uniform
      //  double  timeValue = glfwGetTime();
       // float greenValue = static_cast<float>(sin(timeValue) / 2.0 + 0.5);
       // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
       // glUniform4f(vertexColorLocation, greenValue,0,0,0);
        
        // bind texture
        float timeValue = glfwGetTime();
     
        grid::reupdatechunkborders();
        grid::load();
      
            blockrender::initdatabuffer();
        
      
        gameobject::deleteobjs();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
