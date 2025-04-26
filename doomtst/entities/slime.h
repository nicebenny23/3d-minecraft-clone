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
#ifndef slime_HPP
#define slime_HPP

array<navnode> getneighborslime(navnode& node);
struct slimemove :gameobject::component {
    bool ctype;
    float timesincejump;
    void jump() {
        owner->getcomponent<rigidbody>().velocity.y = 9;
    }
    void start() {

        timesincejump = 1;
    }
    void update() {


        timesincejump -= CtxName::ctx.Time->dt;
        Vector3 pos = objutil::toent(owner ).transform.position;
        v3::Vector3 headed = owner->getcomponent<navigator>().headed;
        
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

               objutil::toent(owner).transform.position += normal(gotopos) * CtxName::ctx.Time->dt*2;

           }
           else {

               objutil::toent(owner).transform.position += normal(gotopos) * CtxName::ctx.Time->dt * 2;

               }
           }

    }
};

inline Ent::entityref createslime(v3::Vector3 pos,bool type) {

    Ent::entityref refmodel = CtxName::ctx.EntMan->CreateEntity(pos, "slime");
   
  //  refmodel.toent()->getcomponent<model>().add("slime2.obj", "images\\slimetex.png");
    if (type)
    {

        refmodel.toent()->addcomponent<model>()->add("slime2.obj", "images\\slimetexred.png");

    }
    else {
        refmodel.toent()->addcomponent<model>()->add("slime2.obj", "images\\slimetex.png");

    }
    refmodel.toent()->addcomponent<loottable>()->addelem(slimeballitem, 1, false);
    float hp= 9;
    if (type == true) {
        hp = 15;

    }
    refmodel.toent()->addcomponent<estate>(hp, true);
    refmodel.toent()->addcomponent<aabb::Collider>(zerov, blockscale/2, true);
    float dmg = 3;
    if (type == true) {
        dmg = 5;

    }
    refmodel.toent()->addcomponent<dmgonhit>(dmg,"player",dmg*2);
    refmodel.toent()->addcomponent<rigidbody>();
    refmodel.toent()->addcomponent<navigator>(player::goblin, getneighborslime);
    refmodel.toent()->addcomponent<slimemove>();

    refmodel->getcomponent<slimemove>().ctype = type;
    refmodel.toent()->transform.scale = blockscale/2;
    return refmodel;
}



#endif // !slime_HPP
