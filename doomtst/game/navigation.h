#include <cmath>
#include "../world/grid.h"
#include "../util/dir.h"
#include <memory>
#include "entity.h"
#ifndef  nav_HPP
#define nav_HPP


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
        if (parentnode == nullptr)
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

    bool operator==(const navnode& other) {
        return pos == other.pos;
    }


};
array<navnode> getneighborsdefault(navnode& node);
bool normaltestfunc(Coord  pos, int dir);
struct navigator:gameobject::component
{
    navigator(entityname::entityref parentref, array<navnode> (*testfunc)(navnode& pos));
    bool isblockvalid;
    v3::Vector3 esize;
    float timetillupdate;
    v3::Vector3 headed;
    array<navnode> (*testfunction)(navnode& pos);
    entityname::entityref goingtwords;
    void calcpath();
    bool noblockinrange(Coord pos);
    void update();
};

array<navnode> astarpathfinding(navnode start, navnode goal, array<navnode> (*getconnected)(navnode& pos));
#endif // ! nav_HPP
