#pragma once
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

array<navnode> getneighborslime(navnode& node);
struct slimemove : gameobject::component {
    bool ctype;
    float timesincejump;
    void jump() {
        owner.getcomponent<rigidbody>().velocity.y = 9;
    }
    void start() {

        timesincejump = 1;
    }
    void update() {


        timesincejump -= CtxName::ctx.Time->dt;
        Vector3 pos = owner.transform().position;
        v3::Vector3 headed = owner.getcomponent<navigator>().headed;
        
            v3::Vector3 gotopos = headed - pos;
            if (gotopos!=zerov)
            {

           if (gotopos.y>.6f)
            {
                if (timesincejump<0)
                {

                    timesincejump = 3;
                    jump();
                }
            }
          
           if (ctype)
           {

               owner.transform().position += normal(gotopos) * CtxName::ctx.Time->dt*2;

           }
           else {

               owner.transform().position += normal(gotopos) * CtxName::ctx.Time->dt * 2;

               }
           }

    }
};

inline gameobject::obj createslime(v3::Vector3 pos,bool type) {

    gameobject::obj refmodel = CtxName::ctx.OC->CreateEntity(pos);
   
  //  refmodel.getcomponent<model>().add("slime2.obj", "images\\slimetex.png");
    if (type)
    {

        refmodel.addcomponent<model>()->add("slime2.obj", "images\\slimetexred.png");

    }
    else {
        refmodel.addcomponent<model>()->add("slime2.obj", "images\\slimetex.png");

    }
    refmodel.addcomponent<loottable>()->addelem(slimeballitem, 1, false);
    float hp= 9;
    if (type == true) {
        hp = 15;

    }
    refmodel.addcomponent<estate>(hp, true);
    refmodel.addcomponent<aabb::Collider>(zerov, blockscale/2, true);
    float dmg = 3;
    if (type == true) {
        dmg = 5;

    }
    refmodel.addcomponent<dmgonhit<player::player_tag>>(dmg,dmg*2);
    refmodel.addcomponent<rigidbody>();
    refmodel.addcomponent<navigator>(player::goblin, getneighborslime);
    refmodel.addcomponent<slimemove>();

    refmodel.getcomponent<slimemove>().ctype = type;
    refmodel.transform().scale = blockscale / 2;
    return refmodel;
}



 // !slime_HPP
