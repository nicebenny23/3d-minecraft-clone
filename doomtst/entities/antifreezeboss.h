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

enum bosstate {
    following=0,
    shooting=1,
    ramming=2,
};
struct firedaggerfinalboss :gameobject::component {
    float timetillshoot;
    int shotsleft = 0;
    bosstate state;
    void update(){
        Transform& currtransform = owner->getcomponent<model>()[0].transform;
        Vector3 pos = player::goblin->transform.position;
        if (state == following)
        {


      
            //currtransform.position = pos;
            currtransform.orient(pos);
            currtransform.position += currtransform.getnormaldirection() * .02f;

            if (random() < timename::smoothdt * 1 / 20.f) {
                state = shooting;
                timetillshoot = .1;
                shotsleft = 11;
            }
            if (random() > 1-timename::smoothdt * 1 / 20.f) {
                state = ramming;
                timetillshoot = 1.5;
     
            }
        }
        if (state == shooting)
        {
            currtransform.orientbase(Vector3(0, 1, 0));
            currtransform.position += currtransform.getnormaldirection()*timename::dt * 4;

            //currtransform.position += Vector3(1, 0, 0)*.002f;
            if (timetillshoot<0&&0<shotsleft)
            {

                v3::Vector3 pos = owner->getcomponent<model>()[0].transform.position + owner->getcomponent<model>()[0].transform.getnormaldirection() * 4+Vector3(random(),random(),random());
                Vector3 ppos = player::goblin.toent()->transform.position;
                Vector3 velocity = normal(pos-ppos) * -6+(Vector3(random(),random(),random())-unitv/2);
                spawndagger(pos, velocity, 0);
                shotsleft--;
                timetillshoot = .1;
            }
            if (shotsleft==1)
            {
                state = following;
            }
            timetillshoot -= timename::smoothdt;
        }
        if (state == ramming)
        {



            //currtransform.position = pos;
            currtransform.orient(pos);
            currtransform.position += currtransform.getnormaldirection() * timename::dt*9;
            timetillshoot -= timename::smoothdt;
            if (timetillshoot<0)
            {
                state = following;
            }

        }
    }
    void destroy() {

        player::goblin->transform.position = Vector3(0, 300, 0);

        blockrender::enablelighting = true;

    }

};
inline entityname::entityref createfinalboss(v3::Vector3 pos) {
    
    entityname::entityref refmodel = entityname::createentity(zerov, "enemy");
    refmodel.toent()->addcomponentptr<model>();
    Coord cpos = pos;
    //  refmodel.toent()->getcomponent<model>().add("slime2.obj", "images\\slimetex.png");
    for (int i = 0; i <160; i++)
    {
        refmodel.toent()->getcomponent<model>().add("objs\\finalboss.obj", "images\\bosstex.png", Vector3(i, 0, 0)+cpos);
        refmodel.toent()->addcomponentptr<aabb::Collider>(Vector3(i,0,0)+cpos, unitscale, true)->isunmovable=true;

        refmodel.toent()->getcomponent<model>()[i].transform.yaw = 0;
    }
    refmodel.toent()->addcomponent<firedaggerfinalboss>();
    refmodel.toent()->addcomponent<estate>(100, false);

    refmodel.toent()->addcomponent<dmgonhit>(4, "player", 6);
    refmodel.toent()->addcomponentptr<loottable>()->addelem(playertpsword, 512);
   // refmodel.toent()->addcomponent<rigidbody>();
    refmodel.toent()->transform.scale = unitv;
    refmodel.toent()->addcomponent<worm>(160,1);
    refmodel->canbedestroyed = false;
    return refmodel;
}



#endif // !slime_HPP
