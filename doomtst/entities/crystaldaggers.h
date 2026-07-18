#include "onhit.h"
#include "../game/navigation.h"
#include "../game/collision.h"
#include "../game/rigidbody.h"


#include "../player/player.h"
#include "../items/item.h"
#include "../items/loottable.h"
#pragma once 
inline ecs::obj spawndagger(v3::Point3 pos,v3::Vec3 velocity,float gravscale=.4f) {

	//ecs::obj refmodel = player::goblin.world().spawn_empty();
	//refmodel.add_component<core::LocalTransform>(pos);
  //refmodel.add_component<Model>("objs\\crystaldagger.obj", "images\\crystaldagger.png");

 // aabb::DynamicColliderRecipe().apply(refmodel);
    
 //   refmodel.add_component<Health::DamageOnHit>(3,  7);
    //refmodel.add_component<rigidbody>().velocity=velocity;
  
  //  refmodel.add_component<rotate_to_velocity>();

   // refmodel.get_component<rigidbody>().gravityscale=gravscale;
    //.get_component<rigidbody>().friction = .7f *gravscale/ .4f  ;
 //   refmodel.get_component<core::LocalTransform>().transform.scale = blockscale / 2;
  //  return refmodel;
}



 // !slime_HPP
