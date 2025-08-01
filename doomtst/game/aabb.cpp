#include "aabb.h"
#include "../util/geometry.h"
//simple aabb class without collision implemented 
namespace aabb {
    
    Sparse::PackedSet<Collider*> Colliderlist;
    void initCollider()
    {
        Colliderlist = Sparse::PackedSet<Collider*>();
        
    }
    v3::Vec3 collideaabb(Collider p1, Collider p2)
    {
        return geometry::collidebox(p1.globalbox(), p2.globalbox());

     }
        

   
    void Collider::destroy_hook() 
    {
        //if index is not null which implies that ojbect is nto null
        if (in_list)
        {
            Colliderlist.erase(this);
            
        }
    }
    
    Collider::Collider(const v3::Vec3& objcenter, const v3::Vec3& objscale,bool appendtolist, bool iseffector)
    {
        effector = iseffector;
        utype = gameobject::updatenone;
        box.center = objcenter;
        box.scale = objscale;
        
        in_list = appendtolist;
        if (appendtolist) {
            Colliderlist.push(this);
        
        }
     

    }



    bool  aabbboxintersect(geometry::Box p1, Collider& p2)
    {
        return geometry::boxboxintersect(p1, p2.globalbox());
    }

}
