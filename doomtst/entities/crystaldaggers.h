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
#include "rotatetovelocity.h"
#ifndef crystaldaggers_HPP
#define crystaldaggers_HPP

inline entityname::entityref spawndagger(v3::Vector3 pos,v3::Vector3 velocity,float gravscale=.4f) {

    entityname::entityref refmodel = entityname::createentity(pos, "dagger");
  refmodel.toent()->addcomponent<model>()->add("objs\\crystaldagger.obj", "images\\crystaldagger.png");
   
    refmodel->addcomponent<aabb::Collider>(zerov, unitscale / 2, true);

    refmodel.toent()->addcomponent<dmgonhit>(3, "entity", 7);
    refmodel.toent()->addcomponent<destroyonhit>("");
    refmodel.toent()->addcomponent<rigidbody>()->velocity=velocity;
  
    refmodel->addcomponent<rotatetwordsvel>();

    refmodel.toent()->getcomponent<rigidbody>().gravityscale=gravscale;
    refmodel.toent()->getcomponent<rigidbody>().friction = .7f *gravscale/ .4f  ;
    refmodel.toent()->transform.scale = unitscale / 2;
    return refmodel;
}



#endif // !slime_HPP
