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
inline ecs::obj spawndagger(v3::Vec3 pos,v3::Vec3 velocity,float gravscale=.4f) {

	ecs::obj refmodel = CtxName::ctx.OC->spawn_empty();
	refmodel.add_component<ecs::transform_comp>(pos);
  refmodel.add_component<model>().add("objs\\crystaldagger.obj", "images\\crystaldagger.png");
   
    refmodel.add_component<aabb::Collider>(zerov, blockscale / 2, true);

    refmodel.add_component<dmgonhit<estate>>(3,  7);
    //refmodel.add_component<destroyonhit<>>();
    refmodel.add_component<rigidbody>().velocity=velocity;
  
    refmodel.add_component<rotatetwordsvel>();

    refmodel.get_component<rigidbody>().gravityscale=gravscale;
    refmodel.get_component<rigidbody>().friction = .7f *gravscale/ .4f  ;
    refmodel.get_component<ecs::transform_comp>().transform.scale = blockscale / 2;
    return refmodel;
}



 // !slime_HPP
