#include "dmgonhit.h"
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
    float timesincejump;
    void jump() {
        owner->getcomponent<rigidbody>().velocity.y = 9;
    }
    void start() {

        timesincejump = 1;
    }
    void update() {


        timesincejump -= timename::smoothdt;
        Vector3 pos = objutil::toent(owner ).transform.position;
        v3::Vector3 headed = owner->getcomponent<navigator>().headed;
        
            v3::Vector3 gotopos = headed -voxtra::getcurrvoxel( pos);
            if (gotopos!=zerov)
            {

           if (gotopos.y>.2f)
            {
                if (timesincejump<0)
                {
                    timesincejump = 3;
                    jump();
                }
            }
          
            objutil::toent(owner).transform.position += normal(gotopos) * timename::smoothdt;
            }

    }
};

inline entityname::entityref createslime(v3::Vector3 pos) {

    entityname::entityref refmodel = entityname::createentity(pos, "frjiofiuje");
    refmodel.toent()->addcomponent<model>();
   // refmodel.toent()->getcomponent<model>().add("slime2.obj", "images\\slimetex.png");
    refmodel.toent()->getcomponent<model>().add("newtest.obj", "images\\slimetex.png");
    refmodel.toent()->addcomponentptr<loottable>()->addelem(slimeballitem, 1, false);
    refmodel.toent()->addcomponent<estate>(10, false);
    refmodel.toent()->addcomponent<aabb::Collider>(zerov, unitscale/2, true);

    refmodel.toent()->addcomponent<dmgplayeronhit>(6);
    refmodel.toent()->addcomponent<rigidbody>();
    refmodel.toent()->addcomponent<navigator>(player::goblin, getneighborslime);
    refmodel.toent()->addcomponent<slimemove>();
    refmodel.toent()->transform.scale = unitscale/2;
    return refmodel;
}



#endif // !slime_HPP
