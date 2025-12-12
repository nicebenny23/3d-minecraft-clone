#include "onhit.h"
#include "../game/navigation.h"
#include "../game/collision.h"
#include "../game/rigidbody.h"
#include "../renderer/model.h"
/*#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../player/player.h"
#include "../items/item.h"
#include "../items/loottable.h"
#include "../game/wormobj.h"
#include "crystaldaggers.h"
#pragma once 
enum bosstate {
    following=0,
    shooting=1,
    ramming=2,
};
struct firedaggerfinalboss : ecs::component{
    float timetillshoot;
    int shotsleft = 0;
    bosstate state;
    void update(){
        Transform& currtransform = owner->get_component<model>()[0].transform;
        Vec3 pos = player::goblin->transform.position;
        if (state == following)
        {


      
            //currtransform.position = pos;
            currtransform.Orient(pos);
            currtransform.position += currtransform.getnormaldirection() * .02f;

            if (random() < CtxName::ctx.Time->dt * 1 / 20.f) {
                state = shooting;
                timetillshoot = .1;
                shotsleft = 11;
            }
            if (random() > 1-CtxName::ctx.Time->dt * 1 / 20.f) {
                state = ramming;
                timetillshoot = 1.5;
     
            }
        }
        if (state == shooting)
        {
            currtransform.OrientDir(Vec3(0, 1, 0));
            currtransform.position += currtransform.getnormaldirection()*CtxName::ctx.Time->dt * 4;

            //currtransform.position += Vec3(1, 0, 0)*.002f;
            if (timetillshoot<0&&0<shotsleft)
            {

                v3::Vec3 pos = owner->get_component<model>()[0].transform.position + owner->get_component<model>()[0].transform.getnormaldirection() * 4+Vec3(random(),random(),random());
                Vec3 ppos = player::goblin.toent()->transform.position;
                Vec3 velocity = normal(pos-ppos) * -6+(Vec3(random(),random(),random())-unitv/2);
                spawndagger(pos, velocity, 0);
                shotsleft--;
                timetillshoot = .1;
            }
            if (shotsleft==1)
            {
                state = following;
            }
            timetillshoot -= CtxName::ctx.Time->dt;
        }
        if (state == ramming)
        {



            //currtransform.position = pos;
            currtransform.Orient(pos);
            currtransform.position += currtransform.getnormaldirection() * CtxName::ctx.Time->dt*9;
            timetillshoot -= CtxName::ctx.Time->dt;
            if (timetillshoot<0)
            {
                state = following;
            }

        }
    }
    void destroy() {

        player::goblin->transform.position = Vec3(0, 300, 0);


    }

};
inline Ent::entityref createfinalboss(v3::Vec3 pos) {
    
    Ent::entityref refmodel = CtxName::ctx.EntMan->CreateEntity(zerov, "enemy");
    refmodel.toent()->add_component<model>();
    Coord cpos = pos;
    //  refmodel.toent()->get_component<model>().add("slime2.obj", "images\\slimetex.png");
    for (int i = 0; i <160; i++)
    {
        refmodel.toent()->get_component<model>().add("objs\\finalboss.obj", "images\\bosstex.png", Vec3(i, 0, 0)+cpos);
       refmodel.toent()->get_component<model>()[i].transform.yaw = 0;
    }
    refmodel.toent()->add_component<firedaggerfinalboss>();
    refmodel.toent()->add_component<estate>(100, false);

    refmodel.toent()->add_component<dmgonhit>(4, "player", 6);
    refmodel.toent()->add_component<loottable>()->addelem(playertpsword, 512);
   // refmodel.toent()->add_component<rigidbody>();
    refmodel.toent()->transform.scale = unitv;
    refmodel.toent()->add_component<worm>(160,1);
    refmodel->canbedestroyed = false;
    return refmodel;
}



 // !slime_HPP
 */