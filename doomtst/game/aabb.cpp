#include "aabb.h"
#include "../util/geometry.h"
//simple aabb class without collision implemented 
namespace aabb {

    dynamicarray::array<Collider*> Colliderlist;
    void initCollider()
    {
        Colliderlist = dynamicarray::array<Collider*>(1000);
        for (int i = 0; i < 1000; i++)
        {
            Colliderlist[i] = nullptr;
        }
    }
    bool aabbsintersect(Collider & p1, Collider & p2)
    {
        if (abs(p1.center.x - p2.center.x) < p1.scale.x + p2.scale.x)
        {
            if (abs(p1.center.y - p2.center.y) < p1.scale.y + p2.scale.y)
            {
                if (abs(p1.center.z - p2.center.z) < p1.scale.z + p2.scale.z)
                {
                    return true;
                }
            }
        }
        return false;
    }
    
    v3::Vector3 collideaabb(Collider p1, Collider p2)
    {
        int sgnx = p1.center.x > p2.center.x?1 : -1;
        int sgny = p1.center.y > p2.center.y ? 1 : -1;
        int sgnz = p1.center.z> p2.center.z ? 1 : -1;
        if (aabbsintersect(p1, p2)) {

            float xdepth = sgnx * (p1.scale.x + p2.scale.x) - (p1.center.x - p2.center.x);
            float ydepth = sgny * (p1.scale.y + p2.scale.y) - (p1.center.y - p2.center.y);
            float zdepth = sgnz * (p1.scale.z + p2.scale.z) - (p1.center.z - p2.center.z);
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
        pos -= center;
        if (abs(pos.x)<=scale.x)
        {
            if (abs(pos.y) <= scale.y)
            {
                if (abs(pos.z) <= scale.z)
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
            Colliderlist[index]=nullptr;
        }
    }

    Collider::Collider(const v3::Vector3& objcenter, const v3::Vector3& objscale,bool appendtolist, bool iseffector)
    {
        effector = iseffector;
        hasrigidbody = false;
        center = objcenter;
        scale = objscale;
        index = -1;
        if (appendtolist) {
           
            while (index == -1)
            {
                
                int l = randomint(1000);
                if (Colliderlist[l] == nullptr)
                {
                    Colliderlist[l] = this;
                    index = l;
                }

            }
        }
    }
    aabbraycolinfo Collider::distanceonray(ray fray)
    {
        aabbraycolinfo toreturn = aabbraycolinfo();
        
        v3::Vector3 dir = fray.end - fray.start;

        //not actually max
        float xval1 = (center.x - scale.x - fray.start.x) / dir.x;
        float xval2 = (center.x + scale.x - fray.start.x) / dir.x;
        float maxxval = fmax(xval1, xval2);
        float minxval = fmin(xval1, xval2);
        float yval1 = (center.y - scale.y - fray.start.y) / dir.y;
        float yval2 = (center.y + scale.y - fray.start.y) / dir.y;
        float maxyval = fmax(yval1, yval2);
        float minyval = fmin(yval1, yval2);
        float zval1 = (center.z - scale.z - fray.start.z) / dir.z;
        float zval2 = (center.z + scale.z - fray.start.z) / dir.z;
        float maxzval = fmax(zval1, zval2);
        float minzval = fmin(zval1, zval2);
        float actualminval = fmax(fmax(minxval, minyval), minzval);
        float actualmaxval = fmin(fmin(maxxval, maxyval), maxzval);
        
        if (actualminval < actualmaxval)
        {
            float closestt = actualminval;
            if (0 < actualminval)
            {
                toreturn.collided = true;
                toreturn.intersectionpoint = dir * closestt + fray.start;

                toreturn.dist = v3::distance(fray.start, toreturn.intersectionpoint);


            }

        }

        return toreturn;
        
    }




    bool  aabbboxintersect(geometry::Box p1, Collider& p2)
    {
        if (abs(p1.center.x - p2.center.x) < p1.scale.x + p2.scale.x)
        {
            if (abs(p1.center.y - p2.center.y) < p1.scale.y + p2.scale.y)
            {
                if (abs(p1.center.z - p2.center.z) < p1.scale.z + p2.scale.z)
                {
                    return true;
                }
            }
        }
        return false;
    }

}
