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
#ifndef slime_HPP
#define slime_HPP

inline entityname::entityref spawndagger(v3::Vector3 pos,v3::Vector3 velocity) {

    entityname::entityref refmodel = entityname::createentity(pos, "dagger");
  refmodel.toent()->addcomponentptr<model>()->add("objs\\crystaldagger.obj", "images\\crystaldagger.png");
   
    refmodel.toent()->addcomponent<aabb::Collider>(zerov, unitscale / 2, true);

    refmodel.toent()->addcomponent<dmgonhit>(6, "entity", true);
    refmodel.toent()->addcomponent<destroyonhit>(6, "");
    refmodel.toent()->addcomponentptr<rigidbody>()->velocity=velocity;
  
    refmodel->addcomponent<rotatetwordsvel>();
    refmodel.toent()->transform.scale = unitscale / 2;
    return refmodel;
}



#endif // !slime_HPP
