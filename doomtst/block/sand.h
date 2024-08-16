#include "block.h"
#include "../world/grid.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../items/loottable.h"
#include "../game/gameobject.h"
#ifndef sandob_HPP
#define sandob_HPP

struct sandfall : gameobject::component {
    float timetilltest = .4f;
    
    void update() {
        if (timetilltest > 0) {

            timetilltest -= tick::tickdt;
            return;
        }
        timetilltest = .29f;
        Coord pos = objutil::toblk(owner).pos;
        Coord lowpos = pos - Coord(0, 1, 0);

  
        block* obj = grid::getobjatgrid(lowpos, true);

        // Check if the block below is empty or can be replaced
        if (obj != nullptr) {
            if (!obj->attributes.solid)
            {

                gridutil::setblock(lowpos, minecraftsand);
                gridutil::setblock(pos, minecraftair); 
            }
        }
    
    }
 
    sandfall() {
        utype = gameobject::updatetick;

    }
    ~sandfall() = default;
};

inline void sandinit(blockname::block* blk) {
    blk->mesh.setfaces(sandtex, sandtex, sandtex, sandtex, sandtex, sandtex);
    blk->attributes.solid = true;
    blk->attributes.transparent = false;
    blk->emitedlight = 0;
    blk->mininglevel = .5f;
    blk->mesh.scale = blockname::unitscale;
    blk->createaabb();
    blk->addcomponent<sandfall>();
    
     blk->addcomponent<loottable>()->addelem(sanditem,1, false);
}

inline void sanddelete(blockname::block* blk) {
    blk->removecomponent<aabb::Collider>();
    blk->removecomponent<sandfall>();
     blk->removecomponent<loottable>();
}

#endif // sandob_HPP