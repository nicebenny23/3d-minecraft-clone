#include "aabb.h"
#include "../util/geometry.h"
//simple aabb class without collision implemented 
namespace aabb {
    
    Sparse::SparseSet<Collider*> Colliderlist;
    void initCollider()
    {
        Colliderlist = Sparse::SparseSet<Collider*>();
        
    }
    v3::Vector3 collideaabb(Collider p1, Collider p2)
    {
        return geometry::collidebox(p1.globalbox(), p2.globalbox());

     }
        

   
    void Collider::ondestroy()
    {
        //if index is not null which implies that ojbect is nto null
        if (index!=-1)
        {
            Colliderlist.remove(this);
            
        }
    }
    
    Collider::Collider(const v3::Vector3& objcenter, const v3::Vector3& objscale,bool appendtolist, bool iseffector)
    {
        effector = iseffector;
        utype = gameobject::updatenone;
        box.center = objcenter;
        box.scale = objscale;
        
        index = -1;
        if (appendtolist) {
            Colliderlist.push(this);
        
        }
     

    }



    bool  aabbboxintersect(geometry::Box p1, Collider& p2)
    {
        return geometry::boxboxintersect(p1, p2.globalbox());
    }

}
