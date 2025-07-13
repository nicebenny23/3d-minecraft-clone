#pragma once
#include "navigation.h"
#include "objecthelper.h"
#include "../util/time.h"
#include "../util/dynamicarray.h"
#include "../util/dir.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"

// Helper function for heuristic calculation (Manhattan distance)
float appdist(const navnode& a, const navnode& b) {
    return v3::mag(Vec3( a.pos- b.pos));
}

array<navnode> getneighborsdefault( navnode& node) {
    
    array<navnode> neighbors= array<navnode>();
    for (auto dir: Dir::Directions3d) {
        if (dir==Dir::down3d||dir==Dir::up3d) {
            continue;
        }
        Coord point= Coord(dir.ToVec() + node.pos);

        v3::Vec3 center = point + unitv / 2;
        v3::Vec3 scale = blockscale * v3::Vec3(1.1, .9f, 1.1);
        geometry::Box bx = geometry::Box(center, scale);
        if (!voxtra::Boxcollwithgrid(bx))
        {
            neighbors.push(navnode(point));
        }
           
        
    }
    return array<navnode>( neighbors);
}

array<navnode> reconstructpath(navnode* node) {
    array<navnode> path;
 
    while (node != nullptr) {
        path.push(*new navnode(*node));
       
            node = node->parent;
    }
    // Reverse the path array
    int searchlength = path.length / 2;
    for (int i = 0; i < searchlength; i++) {
        std::swap( path[i],path[ path.length - 1 - i]);
         }
    return Cont::array<navnode>( path);
}

bool normaltestfunc(Coord pos, int dir)
{

    return false;

}

array<navnode> astarpathfinding(navnode start, navnode goal, array<navnode> (*getconnected)(navnode& pos)) {
    if (CtxName::ctx.Grid->GetChunk(goal.pos)==nullptr)
    {
return        array<navnode>();
    }
    array<navnode> openlist;
    array<navnode> closedlist;
    array<navnode*> todeallocatelist;
    start.gcost = dist(start.pos, goal.pos);

    openlist.push(start);
    
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
        todeallocatelist.push(newnode);

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
            return array<navnode>( toret);
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
            return array<navnode>(toret);
        }

        closedlist.push(current);

        array<navnode> neighbors = getconnected(current);
        for (int i = 0; i < neighbors.length; i++) {
            navnode& neighbor = neighbors[i];

            // Check if neighbor is in closed list
            bool inclosedlist = false;
            for (int j = 0; j < closedlist.length; j++) {
                if (closedlist.at(j) == neighbor) {
                    inclosedlist = true;
                    break;
                }
            }
            if (inclosedlist) {
                continue;
            }
           // float potentialg = current.gcost + 1;
            
            float potentialg = current.gcost + dist(current.pos,neighbor.pos);

            // Check if neighbor is in open list
            bool inopenlist = false;
            for (int j = 0; j < openlist.length; j++) {
                if (neighbor==openlist.at(j) ) {

                    inopenlist = true;
                    //updates gcost to be shorter
                    if (potentialg < openlist.at(j).gcost) {
                        openlist.at(j).gcost = potentialg;
                        openlist.at(j).parent = newnode;
                    }
                    break;
                }
            }

            if (inopenlist) {
                continue;
            }
                neighbor.gcost = potentialg;
                neighbor.hcost = appdist(neighbor, goal);
                neighbor.parent = newnode;
                openlist.push(neighbor);
            
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

navigator::navigator(gameobject::obj parentref, array<navnode>(*testfunc)(navnode& pos))
{
    priority = 4411;
    timetillupdate = 0;
    goingtwords = parentref;
    testfunction = testfunc;
}

Vec3 transformnormal(Vec3 pos, Vec3 scale)
{
    pos.y = floor(pos.y);
    pos = (pos);
    Vec3  low = pos - scale;
    Vec3  high = pos + scale;

    for (int xind = low.x; xind < high.x; xind++)
    {

        for (int zind = low.z; zind < high.z; zind++)
        {
           

                Vec3 center = Vec3(xind, pos.y - 1, zind);
                geometry::Box posbx = geometry::Box(center+unitv/2, blockscale);
                if (!voxtra::Boxcollwithgrid(posbx))
                {
                    continue;
                }
                center += Vec3( 0,1,0);
                posbx.center += Vec3(0, 1, 0);
                if (voxtra::Boxcollwithgrid(posbx) == true)
                {
                    return Vec3(xind, pos.y, zind);
                }
        }


    }
    return pos;
}

void navigator::calcpath()
{
    
    Coord currpos = CtxName::ctx.Grid->getVoxel( owner.transform().position);

    Vec3 gotopos = CtxName::ctx.Grid->getVoxel(goingtwords.transform().position);
 
    array<navnode>  finding = astarpathfinding(navnode(currpos),navnode(Coord(gotopos)), testfunction);
    if (finding.length>1)
    {
        headed = finding[1].pos+unitv/2;
    }
    else {

        headed = currpos;
    }
    finding.destroy();
}

bool navigator::noblockinrange(Coord pos)
{
    Coord lowest = Coord(pos-unitv * esize);

    Coord heighest=Coord(pos + unitv * esize);

    for (int xind= -lowest.x; xind < heighest.x; xind++)
    {
        for (int yind = -lowest.y; yind < heighest.y; yind++)
        {
            for (int zind = -lowest.z; zind < heighest.z; zind++)
            {
                Coord pos = Coord(xind, yind, zind);
                
            }
        }
    }
    return true;
}

void navigator::update()
{
    float timetillupdatespeed = .3;

    Vec3 gotopos = CtxName::ctx.Grid->getVoxel(goingtwords.transform().position);

    v3::Vec3 loc = owner.transform().position;
    float distance = dist(loc, gotopos);
    float addoffset = Max(0,(sigmoid(distance/ 10)-.5)*2);
    timetillupdatespeed = .3 + addoffset;
    timetillupdate -= CtxName::ctx.Time->dt;
 
    if (timetillupdate<=0||dist2(loc,headed)<.04)
    {
        calcpath();

      //  calcpath();
        timetillupdate =timetillupdatespeed;
    }
  
}
