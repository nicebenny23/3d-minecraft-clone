#pragma once
#include "navigation.h"
#include "objecthelper.h"
#include "../game/time.h"
#include "../util/dynamicarray.h"
#include "../math/dir.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"

// Helper function for heuristic calculation (Manhattan distance)
float appdist(const navnode& a, const navnode& b) {
    return dist2(a.pos,b.pos);
}

array<navnode> getneighborsdefault( navnode& node) {
    
    array<navnode> neighbors= array<navnode>();
    for (auto dir: math::Directions3d) {
        if (dir==math::down_3d||dir==math::up_3d) {
            continue;
        }
        Coord point= Coord(dir.coord() + node.pos);

        v3::Point3 center = point + unitv / 2;
        v3::Scale3 scale = blockscale * v3::Scale3(1.1, .9f, 1.1);
        geo::Box bx = geo::Box(center, scale);
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
    int searchlength = path.length() / 2;
    for (int i = 0; i < searchlength; i++) {
        std::swap( path[i],path[ path.length() - 1 - i]);
         }
    return stn::array<navnode>( path);
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
    start.gcost = dist(start.pos, goal.pos);

    openlist.push(start);
    
    const int maxiter = 200;
    int iter = 0;
    while (openlist.length() > 0) {
        iter += 1;
        // Find the node with the lowest f cost
        stn::Option<size_t> shortestind=stn::None;
        float shortestcost = std::numeric_limits<float>().infinity();
        for (int i = 0; i < openlist.length(); ++i) {
            float nodecost = openlist[i].calcfcost();
            if (nodecost < shortestcost) {
                shortestcost = nodecost;
                shortestind = i;
            }
        }
        if (shortestind == stn::None)
        { 
            break;
        }

        navnode current = openlist[shortestind.unwrap()];
        navnode* newnode = new navnode(current);

        openlist.remove_at(shortestind.unwrap());
        
        //removes it 
        if (iter == maxiter)
        {
            
            array<navnode> toret = reconstructpath(&current);
            return array<navnode>( toret);
        }
    
      
        // checks if goal
        if (current == goal) {
         
         
            array<navnode> toret=reconstructpath(&current);
            return toret;
        }

        closedlist.push(current);

        array<navnode> neighbors = getconnected(current);
        for (int i = 0; i < neighbors.length(); i++) {
            navnode& neighbor = neighbors[i];

            // Check if neighbor is in closed list
            bool inclosedlist = false;
            for (int j = 0; j < closedlist.length(); j++) {
                if (closedlist[j] == neighbor) {
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
            for (int j = 0; j < openlist.length(); j++) {
                if (neighbor==openlist[j] ) {

                    inopenlist = true;
                    //updates gcost to be shorter
                    if (potentialg < openlist[j].gcost) {
                        openlist[j].gcost = potentialg;
                        openlist[j].parent =newnode;
                    }
                    break;
                }
            }

            if (inopenlist) {
                continue;
            }
                neighbor.gcost = potentialg;
                neighbor.hcost = appdist(neighbor, goal);
                neighbor.parent =newnode;
                openlist.push(neighbor);
            
        }
    }
    return array<navnode>(); // No path found
}

navigator::navigator(ecs::obj parentref, array<navnode>(*testfunc)(navnode& pos))
{
    headed_index = 0;
    path_creation_dur=CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().create_dur();
    goingtwords = parentref;
    testfunction = testfunc;
}

v3::Point3 navigator::headed()
{
    if (headed_list.length()<=headed_index)
    {
        return owner().get_component<ecs::transform_comp>().transform.position;
    }

    return headed_list[headed_index];
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
           

				Point3 center = Point3(xind, pos.y - 1, zind);
                geo::Box posbx = geo::Box(center+unitv/2, blockscale);
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
    
    Coord currpos = CtxName::ctx.Grid->getVoxel( owner().get_component<ecs::transform_comp>().transform.position);

	Point3 gotopos = CtxName::ctx.Grid->getVoxel(goingtwords.get_component<ecs::transform_comp>().transform.position);
    array<navnode>  finding = astarpathfinding(navnode(currpos),navnode(Coord(gotopos)), testfunction);
    headed_list.clear();
    headed_index = 0;
    for (navnode& node:finding)
    {
        if (node.pos!=currpos)
        {
            headed_list.push(node.pos + unitv / 2);
        }
    }
    if (headed_list.empty())
    {
        headed_list.push(gotopos);
    }
}

bool navigator::noblockinrange(Coord pos)
{
    Coord lowest = Coord(pos-esize);

    Coord heighest=Coord(pos + esize);

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
    float timetillupdatespeed = .5;

	Point3 gotopos = goingtwords.get_component<ecs::transform_comp>().transform.position;

    v3::Point3 loc = owner().get_component<ecs::transform_comp>().transform.position;
   
    if (dist(loc,headed())<.1f)
    {
        
            path_creation_dur.disable();

       
    }
    if (path_creation_dur.state()!=timename::duration_state::active)
    {
        calcpath();
        path_creation_dur.set(timetillupdatespeed);
    }
  
}

bool navigator::should_update_path()
{
	Point3 headed_pos= goingtwords.get_component<ecs::transform_comp>().transform.position;
  
    return false;
}
