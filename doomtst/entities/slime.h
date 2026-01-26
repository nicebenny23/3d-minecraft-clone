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
struct slimemove : ecs::component{
    bool ctype;
    float timesincejump;
    void jump() {
        owner().get_component<rigidbody>().velocity.y = 9;
    }
    void start() {

        timesincejump = 1;
    }
    void update() {


        timesincejump -= CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt;
        Point3 pos = owner().get_component<ecs::transform_comp>().transform.position;
		Point3 headed = owner().get_component<navigator>().headed();
        
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
       

           owner().get_component<rigidbody>().velocity += v3::Vec3(normal(gotopos).x, 0, normal(gotopos).z) * CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt;
               owner().get_component<ecs::transform_comp>().transform.look_towards(v3::Vec3(gotopos.x,0, gotopos.z));
              owner().get_component<ecs::transform_comp>().transform.yaw = 90 * round(owner().get_component<ecs::transform_comp>().transform.yaw/90);
               
           }

    }
};

inline ecs::obj createslime(v3::Coord pos,bool type) {

    ecs::obj refmodel = CtxName::ctx.Ecs->spawn_empty();
	refmodel.add_component<ecs::transform_comp>(pos);
  
        refmodel.add_component<model>().add("meshes\\cubetest.obj", "images\\slimetex.png");
    
    refmodel.add_component<loot_table>().add("iron_item", 1, false);
 
    refmodel.add_component<estate>(10, true);
    refmodel.add_component<aabb::Collider>(v3::Point3(0,0,0), blockscale, true);
    float dmg = 3;
    if (type == true) {
        dmg = 5;

    }
    refmodel.add_component<dmgonhit<player::player_tag>>(dmg,dmg*2);
    refmodel.add_component<rigidbody>();
    refmodel.add_component<navigator>(player::goblin, getneighborslime);
    refmodel.add_component<slimemove>();

    refmodel.get_component<slimemove>().ctype = type;
    refmodel.get_component<ecs::transform_comp>().transform.scale = blockscale / 2;
    return refmodel;
}



 // !slime_HPP
