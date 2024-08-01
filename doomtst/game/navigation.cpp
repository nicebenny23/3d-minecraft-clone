#include "navigation.h"
#include "objecthelper.h"
#include "../util/time.h"
#include "../util/queue.h"
#include "../util/dynamicarray.h"
#include "../util/dir.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
#include <cmath> // for std::abs

// Helper function for heuristic calculation (Manhattan distance)
float appdist(const navnode& a, const navnode& b) {
    return v3::magnitude(Vector3( a.pos- b.pos));
}

array<navnode> getneighborsdefault( navnode& node) {
    
    array<navnode> neighbors=array<navnode>(6);
    for (int i = 0; i < 6; i++) {
        v3::Vector3 neiborpoint = dirfromint(i) + node.pos;
        if (i == 3) {
            continue;
        }
        if (i== 2)
        {
continue;
        }
        Coord place = neiborpoint;
        v3::Vector3 center = place + unitv / 2;
        v3::Vector3 scale = unitscale * v3::Vector3(1.1, .9f, 1.1);
        geometry::Box bx = geometry::Box(center, scale);
        if (!voxtra::Boxcollwithgrid(bx, voxtra::countnormal))
        {
            neighbors.append(navnode(neiborpoint));
        }
           
        
    }
    return neighbors;
}

array<navnode> reconstructpath(navnode* node) {
    array<navnode> path;
 
    while (node != nullptr) {
        path.append(*new navnode(*node));
       
            node = node->parent;
    }
    // Reverse the path array
    int searchlength = path.length / 2;
    for (int i = 0; i < searchlength; i++) {
        std::swap( path[i],path[ path.length - 1 - i]);
         }
    return path;
}

bool normaltestfunc(Coord pos, int dir)
{

    return false;

}

array<navnode> astarpathfinding(navnode start, navnode goal, array<navnode> (*getconnected)(navnode& pos)) {
    if (grid::chunkatpos(goal.pos.x,goal.pos.y,goal.pos.z)==nullptr)
    {
return        array<navnode>();
    }
    array<navnode> openlist;
    array<navnode> closedlist;
    array<navnode*> todeallocatelist;
    openlist.append(start);
    const int maxiter = 200;
    int iter = 0;

    while (openlist.length > 0) {
        iter += 1;
        // Find the node with the lowest f cost
        int shortestind = -1;
        float shortestcost = 1000;
        for (int i = 0; i < openlist.length; ++i) {
            float nodecost = openlist[i].calcfcost();
            if (nodecost < shortestcost) {
                shortestcost = nodecost;
                shortestind = i;
            }
        }
        if (shortestind == -1)
        {
            openlist.destroy();
            closedlist.destroy();
            break;
        }

        navnode current = openlist[shortestind];
        navnode* newnode = new navnode(current);
        todeallocatelist.append(newnode);

        openlist.deleteind(shortestind);

        //removes it 
        if (iter == maxiter)
        {
            
            array<navnode> toret = reconstructpath(&current);
            for (int i = 0; i < todeallocatelist.length; i++)
            {
                delete todeallocatelist[i];
            }
            closedlist.destroy();
            todeallocatelist.destroy();
            openlist.destroy();
            return toret;
        }
    
      
        // checks if goal
        if (current == goal) {
         
         
            array<navnode> toret=reconstructpath(&current);
            for (int i = 0; i < todeallocatelist.length; i++)
            {
                delete todeallocatelist[i];
            }
            closedlist.destroy();
           todeallocatelist.destroy();
            openlist.destroy();
            return toret;
        }

        closedlist.append(current);

        array<navnode> neighbors = getconnected(current);
        for (int i = 0; i < neighbors.length; i++) {
            navnode* neighbor = &neighbors[i];

            // Check if neighbor is in closed list
            bool inclosedlist = false;
            for (int j = 0; j < closedlist.length; j++) {
                if (closedlist.fastat(j) == *neighbor) {
                    inclosedlist = true;
                    break;
                }
            }
            if (inclosedlist) {
                continue;
            }
           // float potentialg = current.gcost + 1;
            
            float potentialg = current.gcost + distance(current.pos,neighbor->pos);

            // Check if neighbor is in open list
            bool inopenlist = false;
            for (int j = 0; j < openlist.length; j++) {
                if (*neighbor==openlist.fastat(j) ) {

                    inopenlist = true;
                    //updates gcost to be shorter
                    if (potentialg < openlist.fastat(j).gcost) {
                        openlist.fastat(j).gcost = potentialg;
                        openlist.fastat(j).parent = newnode;
                    }
                    break;
                }
            }

            if (inopenlist) {
                continue;
            }
                neighbor->gcost = potentialg;
                neighbor->hcost = appdist(*neighbor, goal);
                neighbor->parent = newnode;
                openlist.append(*neighbor);
            
        }
        neighbors.destroy();
    }
    for (int i = 0; i < todeallocatelist.length; i++)
    {
        delete todeallocatelist[i];
    }
    todeallocatelist.destroy();
    
    return array<navnode>(); // No path found
}

navigator::navigator(entityname::entityref parentref, array<navnode>(*testfunc)(navnode& pos))
{
    priority = 4411;
    timetillupdate = 0;
    goingtwords = parentref;
    testfunction = testfunc;
}

void navigator::calcpath()
{
    
    Coord currpos = voxtra::getcurrvoxel(objutil::toent(owner).transform.position);

    Vector3 gotopos = voxtra::getcurrvoxel(goingtwords.toent()->transform.position);
 
    array<navnode>  finding = astarpathfinding(currpos,Coord(gotopos), testfunction);
    if (finding.length>1)
    {
        headed = finding[1].pos;
    }
    else {

        headed = currpos;
    }
}

bool navigator::noblockinrange(Coord pos)
{
    Coord lowest = pos-unitv * esize;

    Coord heighest= pos + unitv * esize;

    for (int xind= -lowest.x; xind < heighest.x; xind++)
    {
        for (int yind = -lowest.y; yind < heighest.y; yind++)
        {
            for (int zind = -lowest.z; zind < heighest.z; zind++)
            {
                Coord pos = Coord(xind, yind, zind);
                if (grid::issolidatpos(pos.x, pos.y, pos.z, 0))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void navigator::update()
{
    v3::Vector3 loc = objutil::toent(owner).transform.position-objutil::toent(owner).transform.scale;
    timetillupdate -= timename::smoothdt;
    if (timetillupdate<=0||dist2(loc,headed)<.01)
    {
        calcpath();
        timetillupdate =.3f;
    }
  
}
