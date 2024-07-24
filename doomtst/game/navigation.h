#include <cmath>
#include "../world/grid.h"
#include "../util/dir.h"
struct navnode {
    Coord pos;
    float gcost, hcost;
    navnode* parent;
    int pathlen;
    navnode() {
        pos = zeroiv;
        gcost = 0;
        hcost = 0;
        parent = nullptr;
        pathlen = 1;
    }
    navnode(Coord position, navnode* parentnode = nullptr)
      {
        if (parentnode==nullptr)
        {

            pathlen = 0;

        }
        else {
            pathlen = parentnode->pathlen + 1;
        }pos = position;
        gcost = 0;
        hcost = 0;
        parent = parentnode;
    }

    float calcfcost() const {
        return gcost + hcost;
    }

    bool operator==(const navnode& other)  {
        return pos == other.pos;
    }

  
};
array<navnode> astarpathfinding(navnode start, navnode goal);
