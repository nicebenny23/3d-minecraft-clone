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
#pragma once 
inline gameobject::obj spawndagger(v3::Vec3 pos,v3::Vec3 velocity,float gravscale=.4f) {

    gameobject::obj refmodel = CtxName::ctx.OC->spawn_with_transform(pos);
  refmodel.addcomponent<model>().add("objs\\crystaldagger.obj", "images\\crystaldagger.png");
   
    refmodel.addcomponent<aabb::Collider>(zerov, blockscale / 2, true);

    refmodel.addcomponent<dmgonhit<estate>>(3,  7);
    //refmodel.addcomponent<destroyonhit<>>();
    refmodel.addcomponent<rigidbody>().velocity=velocity;
  
    refmodel.addcomponent<rotatetwordsvel>();

    refmodel.getcomponent<rigidbody>().gravityscale=gravscale;
    refmodel.getcomponent<rigidbody>().friction = .7f *gravscale/ .4f  ;
    refmodel.transform().scale = blockscale / 2;
    return refmodel;
}



 // !slime_HPP
