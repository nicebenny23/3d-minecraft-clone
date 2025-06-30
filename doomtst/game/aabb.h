#pragma once
#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../util/dynamicarray.h"
#include "../util/ray.h"
#include "../util/geometry.h"
#include "../util/random.h"

#include "../util/SparseSet.h"
#include <cmath>

#include "objecthelper.h"
#include "transform.h"

namespace aabb {

    
  
    struct Collider : gameobject::component {
        bool effector;
       //local box
        geometry::Box box;
        //world box
        geometry::Box globalbox() {

            geometry::Box global;
            if (owner->type==gameobject::entity)
            {
                Transform transform = objutil::toent(owner).transform;
                global.center = box.center * transform.scale*2 + transform.position;
                global.scale = box.scale * transform.scale*2;
                
            }
            if (owner->type == gameobject::block)
            {
                
                Vector3 scale = objutil::toblk(owner).mesh.box.scale;
                Vector3 pos = objutil::toblk(owner).center();
                global.center = box.center * scale * 2 + pos;
                global.scale = box.scale * scale * 2;

            }
            return global;
        }
        
        short index;
      
        Collider() = default;
        Collider(const v3::Vector3& objcenter, const v3::Vector3& objscale, bool appendtolist,bool iseffector=false);
        ~Collider() = default;

       
        void ondestroy();

        
    };

    void initCollider();
    bool aabbboxintersect(geometry::Box p1, Collider& p2);
   v3::Vector3 collideaabb(Collider p1, Collider p2);

    extern Sparse::SparseSet<Collider*> Colliderlist;
}

