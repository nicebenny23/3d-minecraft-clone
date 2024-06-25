#include "aabb.h"

//simple aabb class without collision implemented 
namespace aabb {

    dynamicarray::array<colrect*> colrectlist;
    void initcolrect()
    {
        colrectlist = dynamicarray::array<colrect*>(1000);
        for (int i = 0; i < 1000; i++)
        {
            colrectlist[i] = nullptr;
        }
    }
    v3::Vector3 collideaabb(colrect p1, colrect p2)
    {
        int sgnx = p1.center.x > p2.center.x?1 : -1;
        int sgny = p1.center.y > p2.center.y ? 1 : -1;
        int sgnz = p1.center.z> p2.center.z ? 1 : -1;
        if (abs(p1.center.x-p2.center.x)<p1.scale.x+p2.scale.x)
        {
            if (abs(p1.center.y - p2.center.y) < p1.scale.y + p2.scale.y)
            {
                if (abs(p1.center.z - p2.center.z) < p1.scale.z + p2.scale.z)
                {

                    float xdepth= sgnx * (p1.scale.x + p2.scale.x)-  (p1.center.x - p2.center.x );
                    float ydepth = sgny * (p1.scale.y + p2.scale.y)-  ( p1.center.y - p2.center.y);
                    float zdepth = sgnz * (p1.scale.z + p2.scale.z)- (p1.center.z - p2.center.z) ;
                    if(abs(xdepth) < abs(ydepth)) {
                        if (abs(xdepth)<abs(zdepth))
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
            }
        }
        return v3::zerov;
    }

    void colrect::destroy()
    {
        if (!gridobj)
        {
            colrectlist[index]=nullptr;
        }
    }

    colrect::colrect(const v3::Vector3& objcenter, const v3::Vector3& objscale,bool gridobj)
    {
        center = objcenter;
        scale = objscale;
        index = -1;
        if (!gridobj) {
           
            while (index == -1)
            {
                int l = randomint(1000);
                if (colrectlist[l] == nullptr)
                {
                    colrectlist[l] = this;
                    index = l;
                }

            }
        }
    }
}
