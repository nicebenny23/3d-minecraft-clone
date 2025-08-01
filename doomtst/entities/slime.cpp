#pragma once
#include "slime.h"

array<navnode> getneighborslime(navnode& node)
{
    array<navnode> neighbors = array<navnode>();
    for (int xind= -1; xind <= 1; xind++)
    {
        for (int yind = -1; yind <= 1; yind++)
        {


            for (int zind = -1; zind <= 1; zind++)
            {
                Coord offset = Coord(xind, yind, zind);

                


                
                //new l0cation
                v3::Coord place= offset + node.pos;
                if (offset.x!=0 && offset.z!=0)
                {
                    continue;  
                }
                //if same
                if (place==node.pos)
                {
                    continue;
                }
                //center of block
                v3::Vec3 center = place + unitv / 2;

                v3::Vec3 scale = blockscale * v3::Vec3(.99f, .99f, .99f);
                //where we are now
                geometry::Box bx = geometry::Box(node.center(), scale);
                
                bool cango = true;
                //if close enigh 
                if (node.gcost > 1.2 && node.hcost >1.2)
                {


                    if (yind != -1)
                    {


                        Coord testblock = place-Coord(0,1,0);
                        if (CtxName::ctx.Grid->getBlock(testblock + node.pos) == nullptr)
                        {
                            continue;
                        }
                    }
                }

                if (1.f< node.gcost)
                {

                    for (int i = 0; i < 3; i++)
                    {
                        bx.center += Vec3(offset[i], i);


                        if (voxtra::Boxcollwithgrid(bx))
                        {
                            cango = false;
                        }
                    }
                }

                if (cango)
                {
                    neighbors.push(navnode(place));
                }
            }


        }
    }


    
    return neighbors;
}
