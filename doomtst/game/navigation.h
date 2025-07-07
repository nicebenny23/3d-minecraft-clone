#pragma once
#include "../util/mathutil.h"
#include "../world/grid.h"
#include "../util/dir.h"

#include "entity.h"


struct navnode {
    Coord pos;

    //distance to objective
    float gcost;

    //distance to start
        float hcost;
    navnode* parent;
    int pathlen;
    navnode() {
        pos = zeroiv;
        gcost = 0;
        hcost = 0;
        parent = nullptr;
        pathlen = 1;
    }
    explicit navnode(Coord position, navnode* parentnode = nullptr)
    {
        if (parentnode == nullptr)
        {

            pathlen = 0;

        }
        else {
            pathlen = parentnode->pathlen + 1;
        }pos = position;
        gcost = 0;
        //distance to objective
        hcost = 0;
        parent = parentnode;
    }

    float calcfcost() const {
        return gcost + hcost;
    }

  

};

inline bool operator==(const navnode& nav1, const navnode& nav2) {
    return (Coord(nav1.pos) == Coord(nav2.pos));
}

array<navnode> getneighborsdefault(navnode& node);
bool normaltestfunc(Coord  pos, int dir);
struct navigator: gameobject::component
{
    navigator(gameobject::obj parentref, array<navnode> (*testfunc)(navnode& pos));
    bool isblockvalid;
    v3::Vector3 esize;
    float timetillupdate;
    v3::Vector3 headed;
    array<navnode> (*testfunction)(navnode& pos);
    gameobject::obj goingtwords;
    void calcpath();
    bool noblockinrange(Coord pos);
    void update();
};

array<navnode> astarpathfinding(navnode start, navnode goal, array<navnode> (*getconnected)(navnode& pos));
