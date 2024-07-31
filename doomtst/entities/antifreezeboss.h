#include "onhit.h"
#include "../game/navigation.h"
#include "../game/collision.h"
#include "../game/rigidbody.h"
#include "../renderer/model.h"
#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../player/player.h"
#include "../items/item.h"
#include "../items/loottable.h"
#include "../game/wormobj.h"
#include "crystaldaggers.h"
#ifndef ultraantifreezefinalboss_HPP
#define ultraantifreezefinalboss_HPP

struct firedaggerfinalboss :gameobject::component {

    void update(){

        if (random() < timename::smoothdt) {
            
            v3::Vector3 pos = owner->getcomponent<model>()[0].transform.position + owner->getcomponent<model>()[0].transform.getnormaldirection()*4;
            Vector3 velocity = normal(pos - player::goblin.toent()->transform.position)*-6;
            spawndagger(pos, velocity, 0);
        }

    }

};
inline entityname::entityref createfinalboss(v3::Vector3 pos) {
    
    entityname::entityref refmodel = entityname::createentity(pos, "enemy");
    refmodel.toent()->addcomponentptr<model>();

    //  refmodel.toent()->getcomponent<model>().add("slime2.obj", "images\\slimetex.png");
    for (int i = 0; i <160; i++)
    {
        refmodel.toent()->getcomponent<model>().add("objs\\finalboss.obj", "images\\bosstex.png", Vector3(i, 0, 0));
        refmodel.toent()->addcomponentptr<aabb::Collider>(Vector3(i,0,0), unitscale, true)->isunmovable=true;

        refmodel.toent()->getcomponent<model>()[i].transform.yaw = 0;
    }
    refmodel.toent()->addcomponent<firedaggerfinalboss>();
    refmodel.toent()->addcomponent<estate>(100, false);

    refmodel.toent()->addcomponent<dmgonhit>(4, "player", 6);
   
   // refmodel.toent()->addcomponent<rigidbody>();
    refmodel.toent()->transform.scale = unitscale*2;
    refmodel.toent()->addcomponent<worm>(160,1);
    refmodel->canbedestroyed = false;
    return refmodel;
}



#endif // !slime_HPP
