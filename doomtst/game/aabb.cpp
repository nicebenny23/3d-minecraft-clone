#include "aabb.h"
#include "../util/geometry.h"
//simple aabb class without collision implemented 
namespace aabb {
    
    ptrmempool<Collider> Colliderlist;
    void initCollider()
    {
        Colliderlist = ptrmempool<Collider>();
        Colliderlist.instantiate(1000, false, true);
        
    }
    v3::Vector3 collideaabb(Collider p1, Collider p2)
    {
        int sgnx = p1.box.center.x > p2.box.center.x?1 : -1;
        int sgny = p1.box.center.y > p2.box.center.y ? 1 : -1;
        int sgnz = p1.box.center.z> p2.box.center.z ? 1 : -1;
        if (geometry::boxboxintersect(p1.box, p2.box)) {

            float xdepth = sgnx * (p1.box.scale.x + p2.box.scale.x) - (p1.box.center.x - p2.box.center.x);
            float ydepth = sgny * (p1.box.scale.y + p2.box.scale.y) - (p1.box.center.y - p2.box.center.y);
            float zdepth = sgnz * (p1.box.scale.z + p2.box.scale.z) - (p1.box.center.z - p2.box.center.z);
            if (abs(xdepth) < abs(ydepth)) {
                if (abs(xdepth) < abs(zdepth))
                {
                    return v3::Vector3(xdepth, 0, 0);
                }
                return v3::Vector3(0, 0, zdepth);
            }
            if (abs(ydepth) < abs(zdepth))
            {
                return v3::Vector3(0, ydepth, 0);
            }
            return v3::Vector3(0, 0, zdepth);
        }
        return v3::zerov;

      }
        


    bool Collider::pointinbox(v3::Vector3 pos)
    {
        pos -= box.center;
        if (abs(pos.x)<=box.scale.x)
        {
            if (abs(pos.y) <= box.scale.y)
            {
                if (abs(pos.z) <= box.scale.z)
                {
                    return true;
                }
            }
        }
        return false;
    }

    void Collider::ondestroy()
    {
        if (owner->type!=gameobject::block)
        {
            Colliderlist.remove(this);
            int l = 1;
        }
    }
    Collider::Collider(const v3::Vector3& objcenter, const v3::Vector3& objscale,bool appendtolist, bool iseffector)
    {
        effector = iseffector;
        utype = gameobject::updatenone;
        box.center = objcenter;
        collideroffset = objcenter;
        box.scale = objscale;
        index = -1;
        if (appendtolist) {
           
                
                    Colliderlist.append(this);
        
        
        }
     

    }
    aabbraycolinfo Collider::distanceonray(ray fray)
    {
        aabbraycolinfo toreturn = aabbraycolinfo();
   
        Vector3 intersectionpoint = geometry::intersectionpoint(box, fray);
        if (intersectionpoint.x!=NaNf)
        {

            toreturn.collided = true;
            toreturn.intersectionpoint = intersectionpoint;

            toreturn.dist = v3::distance(fray.start, toreturn.intersectionpoint);

        

        }

        return toreturn;
 

    }




    bool  aabbboxintersect(geometry::Box p1, Collider& p2)
    {
        return geometry::boxboxintersect(p1, p2.box);
    }

}
