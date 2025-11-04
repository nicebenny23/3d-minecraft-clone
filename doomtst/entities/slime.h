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
        Vec3 pos = owner.transform().position;
        v3::Vec3 headed = owner.getcomponent<navigator>().headed();
        
            v3::Vec3 gotopos = headed - pos;
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
       

           owner.getcomponent<rigidbody>().velocity += v3::Vec3(normal(gotopos).x, 0, normal(gotopos).z) * CtxName::ctx.Time->dt;
               owner.transform().OrientDir(v3::Vec3(gotopos.x,0, gotopos.z));
              owner.transform().yaw = 90 * round(owner.transform().yaw/90);
               
           }

    }
};

inline gameobject::obj createslime(v3::Vec3 pos,bool type) {

    gameobject::obj refmodel = CtxName::ctx.OC->spawn_with_transform(pos);
   
  
        refmodel.addcomponent<model>()->add("meshes\\cubetest.obj", "images\\slimetex.png");
    
    refmodel.addcomponent<loottable>()->addelem(slimeballitem, 1, false);
 
    refmodel.addcomponent<estate>(10, true);
    refmodel.addcomponent<aabb::Collider>(zerov, blockscale, true);
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
