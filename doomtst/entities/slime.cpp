#include "slime.h"

array<navnode> getneighborslime(navnode& node)
{
    array<navnode> neighbors = array<navnode>(6);
    for (int xind= -1; xind <= 1; xind++)
    {
        for (int yind = -1; yind <= 1; yind++)
        {


            for (int zind = -1; zind <= 1; zind++)
            {
                Coord offset = Coord(xind, yind, zind);

                if (yind == 0)
                {


                    if (xind != 0 && zind != 0) {

                        continue;
                    }
                }




                v3::Coord place= offset + node.pos;
                v3::Vector3 center = place + unitv / 2;
                v3::Vector3 scale = blockscale * v3::Vector3(.99f, .99f, .99f) / 2;
                geometry::Box bx = geometry::Box(node.pos + unitv / 2, scale);
                bool cango = true;
                if (node.gcost > 1.2 && node.hcost >1.2)
                {


                    if (yind != -1)
                    {


                        Coord testblock = Coord(offset.x, offset.y - 1, offset.z);
                        //if (CtxName::cxt.Grid->getobjatgrid(testblock + node.pos, false) == nullptr)
                        {
                            continue;
                        }
                    }
                }

                if (!(node.gcost <= 1.f))
                {

                    for (int i = 0; i < 3; i++)
                    {
                        switch (i)
                        {
                        case 0:

                                bx.center += v3::Vector3(0, offset.y, 0);
                                break;
                        case 1:

                            bx.center += v3::Vector3(offset.x, 0, 0);
                            break;
                        case 2:

                            bx.center += v3::Vector3(0, 0, offset.z);
                            break;
                        }
                        if (voxtra::Boxcollwithgrid(bx))
                        {
                            cango = false;
                        }
                    }
                }

                if (cango)
                {
                    neighbors.push(navnode((place)));
                }
            }


        }
    }


    
    return array<navnode>( neighbors);
}
