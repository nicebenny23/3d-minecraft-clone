#include "navigation.h"
#include "objecthelper.h"
#include "../util/time.h"
#include "../util/queue.h"
#include "../util/dynamicarray.h"
#include "../util/dir.h"
#include "../world/grid.h"

#include <cmath> // for std::abs

// Helper function for heuristic calculation (Manhattan distance)
float appdist(const navnode& a, const navnode& b) {
    return v3::magnitude(Vector3( a.pos- b.pos));
}

array<navnode> getneighbors(const navnode& node) {
    
    array<navnode> neighbors=array<navnode>(6);
    for (int i = 0; i < 6; i++) {
      
        if (i == 3) {
            continue;
        }
        if (i == 2)
        {
            continue;
        }Coord place = dirfromint(i) + node.pos;
        block* blkatpos = grid::getobjatgrid(place, true);
        if (blkatpos != nullptr && blkatpos->id == minecraftair) {
            bool exists=true;
            for (int j = 0; j < 6; j++)
            {
                Coord ultraplace = place+dirfromint(j);
               
                if (j == 3) {
                    continue;
                }
                if (j==2)
                {
                    continue;
                }
                block* blkatpos = grid::getobjatgrid(ultraplace, true);
                if (blkatpos!=nullptr)
                {
                    if (blkatpos->id != minecraftair) {

                        exists = false;
                    }
                }
            }
            if (exists)
            {
                block* blkatposdf = grid::getobjatgrid(place+Coord(0,-1,0), true);
                if (blkatposdf != nullptr)
                {
                    if (blkatposdf->id == minecraftair) {

                        exists = false;
                    }
                }
                neighbors.append(navnode(place));
            }
        }
    }
    return neighbors;
}

array<navnode> reconstructpath(navnode* node) {
    array<navnode> path;
    while (node != nullptr) {
        path.append(*node);
        node = node->parent;
    }
    // Reverse the path array
    int searchlength = path.length / 2;
    for (int i = 0; i < searchlength; i++) {
        std::swap( path[i],path[ path.length - 1 - i]);
         }
    return path;
}

array<navnode> astarpathfinding(navnode start, navnode goal) {
    if (grid::chunkatpos(goal.pos.x,goal.pos.y,goal.pos.z)==nullptr)
    {
return        array<navnode>();
    }
    array<navnode> openlist;
    array<navnode> closedlist;

    openlist.append(start);
    const int maxiter = 300;
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
        //removes it 
        if (shortestind==-1||iter==maxiter)
        {
            break;
        }
        navnode current = openlist[shortestind];
        openlist.deleteind(shortestind);

        // checks if goal
        if (current == goal) {
            return reconstructpath(&current);
        }

        closedlist.append(current);

        array<navnode> neighbors = getneighbors(current);
        for (int i = 0; i < neighbors.length; i++) {
            navnode* neighbor = &neighbors[i];

            // Check if neighbor is in closed list
            bool inclosedlist = false;
            for (int j = 0; j < closedlist.length; ++j) {
                if (closedlist[j] == *neighbor) {
                    inclosedlist = true;
                    break;
                }
            }
            if (inclosedlist) {
                continue;
            }

            float potentialg = current.gcost + 1;

            // Check if neighbor is in open list
            bool inopenlist = false;
            for (int j = 0; j < openlist.length; j++) {
                if (*neighbor==openlist[j] ) {

                    inopenlist = true;
                    //updates gcost to be shorter
                    if (potentialg < openlist[j].gcost) {
                        openlist[j].gcost = potentialg;
                        openlist[j].parent = new navnode(current);
                    }
                    break;
                }
            }

            if (inopenlist) {
                continue;
            }
                neighbor->gcost = potentialg;
                neighbor->hcost = appdist(*neighbor, goal);
                neighbor->parent = new navnode(current);
                neighbor->pathlen = current.pathlen + 1;
                openlist.append(*neighbor);
            
        }
    }

    return array<navnode>(); // No path found
}