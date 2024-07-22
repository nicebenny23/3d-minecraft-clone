#include "game/camera.h"
#include "renderer/mesh.h"
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
#include "game/particles.h"
#include "renderer/blockrender.h"
#include "world/managegrid.h"
#include "util/time.h"
#include "game/collision.h"
#include "renderer/uirender.h"
#include "player/playermodification.h"
#include "util/fileloader.h"
#include "renderer/textrender.h"
#include "player/player.h"
#include "renderer/textrender.h"
// settings
#include "renderer/model.h"
const Vector3 spawnpos = glm::vec3(0,0,0);
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
    uirender::newbox("images\\crosshair.png", v2::unitv / 32, v2::zerov,-3);
    userinput::endupdate();
    aabb::initCollider();
    
    inittextarray();
}
void render() {




}
int main()
{

    init();
    player::initplayer();
    entityname::entityref refmodel = entityname::createentity(zerov, "frjiofiuje");
    refmodel.toent()->addcomponent<model>();
    refmodel.toent()->getcomponent<model>().add("slime.obj", "images\\slimetex.png");
    glfwSwapInterval(0);
    refmodel.toent()->addcomponent<Collider>(zerov, unitv , true);
    texture mtex = texture("images\\slimetex.png", png);
  //  meshname::mesh newmehs = *meshname::loadmesh("newtest.obj", mtex,spawnpos);

  //  entityname::entityref emit = entityname::createentity(Vector3(.01f,0,0), "2");
  //  emit.toent()->addcomponent<particleemiter>(3, initbaseparticle);
  // emit.toent()->getcomponent<particleemiter>().tex = mtex;
    //newmehs.yaw=1;

float lastupdate = 0;

while (!window::shouldclose())
    {
    timename::calcfps();

    window::processInput();

    renderer::clear();
    entityname::runupdateloop();
    grid::updateblocks();
    collision::update();
    


    camera::calculateyawandpitch();
    
 
    camera::sendoffviewmatrix();
   
    userinput::endupdate();
    // update shader uniform
    camera::setcamerapos(player::goblin.toent()->transform.position);
        
        
        
    grid::reupdatechunkborders();
    grid::load();
    if (grid::gridchanged())
    {
        gridutil::computeallcover();
        gridutil::redoallighting = true;

    }
    refmodel.toent()->transform.scale = unitv ;
    //refmodel.toent()->transform.yaw += 1 / 30.f;
    
    gridutil::redolighting();
  //meshname::rendermesh(&newmehs);
    blockrender::initdatabuffer(false);
    entityname::runrenderloop();
    //emit.toent()->getcomponent<particleemiter>().renderparticles();

    blockrender::initdatabuffer(true);
    uirender::renderuilist();
    rendertextlist();
    entityname::deleteobjs();
   
    window::swapbuffer();
    glfwPollEvents();
    lastupdate += timename::dt;
        if (lastupdate > 1)
        {

            lastupdate = 0;
           // Vector3 pos1 = human.toent()->pos;
            std::cout <<1/timename::dt << '\n';
        }
    }
   
    glfwTerminate();
    return 0;
}
