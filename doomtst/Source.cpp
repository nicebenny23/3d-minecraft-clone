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
#include "entities/dmgonhit.h"
#include "game/navigation.h"
// settings

#include "renderer/model.h"
#include "world/noise.h"
const Vector3 spawnpos = glm::vec3(0,0,0);
const unsigned int SCR_WIDTH = 4000;
const unsigned int SCR_HEIGHT = 3000;
void init() {
 
    deleteFilesInFolder(std::string("C:/Users/bchar/source/repos/doomtst/doomtst/worldstorage"));

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
    initrandomdirs();
    renderer::load();

  


    uirender::initrenderlist();
    grid::initgrid();
    gridutil::computeallcover();
    gridutil::redolighting();
    uirender::newbox("images\\crosshair.png", v2::unitv / 32, v2::zerov,-3);
    userinput::endupdate();
    aabb::initCollider();
    
    inittextarray();

    player::initplayer();
    glfwSwapInterval(0);
}
void render() {




}
int main()
{

    init();
    entityname::entityref refmodel = entityname::createentity(zerov, "frjiofiuje");
    refmodel.toent()->addcomponent<model>();
    refmodel.toent()->getcomponent<model>().add("newtest.obj", "images\\slimetex.png");
  
    refmodel.toent()->addcomponent<estate>(10,false);
    refmodel.toent()->addcomponent<Collider>(zerov, unitscale, true);
    texture mtex = texture("images\\slimetex.png", png);
  
    refmodel.toent()->addcomponent<dmgplayeronhit>(6);
    refmodel.toent()->addcomponent<rigidbody>();
  
float lastupdate = 0;
refmodel.toent()->transform.position = v3::Vector3(10, 0, 0);
while (!window::shouldclose())
    {
    timename::calcfps();

    window::processInput();

    renderer::clear();

    grid::updateblocks();
    collision::update();

    entityname::runupdateloop();

    collision::sendplayercameraray();
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
   
    if (refmodel.toent()!=nullptr)
    {
        navnode t1 = navnode(Coord(refmodel.toent()->transform.position), nullptr);
        navnode t3 = navnode(player::goblin.toent()->transform.position, nullptr);

        array<navnode> pathl = astarpathfinding(t1, t3);
        refmodel.toent()->transform.scale = unitscale;
        if (pathl.length >=1)
        {
        v3::Vector3 moveoffset = v3::Vector3(pathl[1].pos - pathl[0].pos) * timename::dt;

            refmodel.toent()->transform.position +=moveoffset ;
        }

  //refmodel.toent()->getcomponent<rigidbody>().velocity += refmodel.toent()->transform.getnormaldirection()* timename::smoothdt*3;

    }
   
    //refmodel.toent()->transform.position.x    += 1 / 300.f;
   // refmodel.toent()->transform.position = v3::Vector3(0,0,0);
    gridutil::redolighting();    
  //meshname::rendermesh(&newmehs);
   
    entityname::runrenderloop();
    blockrender::initdatabuffer(false);

    //emit.toent()->getcomponent<particleemiter>().renderparticles();

   
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
