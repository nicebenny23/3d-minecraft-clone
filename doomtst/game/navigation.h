#pragma once
#include "../util/mathutil.h"
#include "../world/grid.h"
#include "../util/dir.h"
#include "entity.h"
#include "../util/time.h"

struct navnode {
    Coord pos;

    //distance to objective
    float gcost;
    v3::Vec3 center() {
        return pos + unitv / 2;
    }
    //distance to start
        float hcost;
    navnode* parent;
    int pathlen;
    navnode() {
        pos = ZeroCoord;
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
struct navigator: ecs::component
{
    navigator(ecs::obj parentref, array<navnode> (*testfunc)(navnode& pos));
    bool isblockvalid;
    v3::Vec3 esize;
    timename::duration path_creation_dur;
    v3::Vec3 headed();
    size_t headed_index;
    array<v3::Vec3> headed_list;
    array<navnode> (*testfunction)(navnode& pos);
    ecs::obj goingtwords;
    void calcpath();
    bool noblockinrange(Coord pos);
    void update();
    bool should_update_path();
};

array<navnode> astarpathfinding(navnode start, navnode goal, array<navnode> (*getconnected)(navnode& pos));
