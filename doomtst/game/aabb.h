#pragma once
#include "../util/vector3.h"
#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include "../util/ray.h"
#include "../util/geometry.h"
#include "../util/random.h"
#include "../block/block.h"
#include "../util/SparseSet.h"
#include <cmath>

#include "objecthelper.h"
#include "transform.h"

namespace aabb {

    
  
    struct Collider : ecs::component{
        bool effector;
       //local box
        geometry::Box box;
        //world box
        geometry::Box globalbox() {

            geometry::Box global;
          
            if (owner().has_component<ecs::transform_comp>())
            {
                Transform transform = owner().get_component<ecs::transform_comp>().transform;
               
                global.scale = box.scale * transform.scale * 2; 
                global.center = box.center * transform.scale * 2 + transform.position;
               

            }
            if (owner().has_component<blockname::block>())
            {
                blockname::block&  blk= owner().get_component<blockname::block>();
                if (blk.id==7)
                {
                    int l = 1;
                }
                Vec3 scale = blk.mesh.box.scale/2/blockname::blockscale;
                Vec3 pos = blk.center();
                global.center = box.center * scale * 2 + pos;
                global.scale = box.scale * scale * 2;

            }
            return global;
        }
        
        bool in_list;
      
        Collider() = default;
        Collider(const v3::Vec3& objcenter, const v3::Vec3& objscale, bool appendtolist,bool iseffector=false);
        ~Collider() = default;

       
        void destroy_hook() override;

        
    };

    void initCollider();
    bool aabbboxintersect(geometry::Box p1, Collider& p2);
    //cannot consify until global box is const
    Option<v3::Vec3> collideaabb( Collider& p1, Collider& p2);

    extern Sparse::PackedSet<Collider*> Colliderlist;
}

