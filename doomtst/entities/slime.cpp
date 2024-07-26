#include "slime.h"

array<navnode> getneighborslime(navnode& node)
{
    array<navnode> neighbors = array<navnode>(6);
    for (int xind= -1; xind <= 1; xind++)
    {
        for (int yind = -1; yind <= 1; yind++)
        {
            for (int zind = -1; zind<= 1; zind++)
            {
                Coord offset = Coord(xind, yind, zind);
                if (magnitude(offset) <= 1.01)
                {
                    if (offset.x==0)
                    {
                        if (offset.z == 0)
                        {
                            continue;
                        }
                    }
                }

        
                v3::Vector3 neiborpoint = offset + node.pos;
                Coord place = neiborpoint;
                v3::Vector3 center = place + unitscale/2;
                v3::Vector3 scale = unitscale * v3::Vector3(.99f,.98,.99f)/2;
                geometry::Box bx = geometry::Box(node.pos +unitv/2, scale);
                bool cango = true;
                if (yind!=-1&&!(node.gcost<= 1.2f||node.hcost<=1.2f))
                {
                    Coord testblock = Coord(offset.x, offset.y-1, offset.z);
                    if (getobjatgrid(testblock+node.pos,false)==nullptr)
                    {
                        continue;
                    }
                  

                }
                for (int i = 0; i < 3; i++)
                {
                    switch (i)
                    {
                    case 0:
                        if (offset.y==0)
                        {
                            continue;
                        }
                        bx.center += v3::Vector3(0, offset.y, 0);
                        break;
                    case 1:
                        if (offset.x == 0)
                        {
                            continue;
                        }
                        bx.center += v3::Vector3(offset.x,0, 0);
                        break;
                    case 2:
                        if (offset.z == 0)
                        {
                            continue;
                        }
                        bx.center += v3::Vector3(0, 0, offset.z);
                        break;
                    }
                    if (voxtra::Boxcollwithgrid(bx, voxtra::countnormal))
                    {
                        cango = false;
                    }
                }
                if (cango)
                {
                    neighbors.append(navnode(neiborpoint));
                }
                   
                
            }
        }
    }


    
    return neighbors;
}
