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
                        if (offset.y == 0)
                        {
                            continue;
                        }
                    }
                }

        
                v3::Vector3 neiborpoint = offset + node.pos;
                Coord place = neiborpoint;
                v3::Vector3 center = place + unitscale/2;
                v3::Vector3 scale = unitscale * v3::Vector3(1.2,.98,1.2)/2;
                geometry::Box bx = geometry::Box(center, scale);
                if (!voxtra::Boxcollwithgrid(bx, true))
                {
                    neighbors.append(navnode(neiborpoint));
                }
            }
        }
    }


    
    return neighbors;
}
