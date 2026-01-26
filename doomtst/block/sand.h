#include "block.h"
#include "../world/grid.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../items/loottable.h"
#include "../game/ecs/game_object.h"
#pragma once 
struct sandfall : ecs::component{
    float timetilltest = .4f;
    
    void update() {
        if (timetilltest > 0) {

          //  timetilltest -= tick::tickdt;
            return;
        }
        timetilltest = .29f;
        Coord pos = owner().get_component<blockname::block>().pos;
        Coord lowpos = pos - Coord(0, 1, 0);

  
        block* obj = CtxName::ctx.Grid->getBlock(lowpos);

        // Check if the block below is empty or can be replaced
        if (obj != nullptr) {
            if (!obj->attributes.solid)
            {

                gridutil::set_block(lowpos, minecraftsand);
                gridutil::set_block(pos, minecraftair); 
            }
        }
    
    }
 
    sandfall() {

    }
    ~sandfall() = default;
};

inline void sandinit(blockname::block& blk) {
    blk.mesh.setfaces(sandtex, sandtex, sandtex, sandtex, sandtex, sandtex);
    blk.attributes.solid = true;
    blk.attributes.transparent = false;
    blk.emitedlight = 0;
    blk.mininglevel = .5f;
    blk.mesh.box.scale = blockname::blockscale;
    blk.createdefaultaabb();
    blk.owner().add_component<sandfall>();
    
     blk.owner().add_component<loot_table>().add("sand", 1, false);
}
