#pragma once
#include "../renderer/uibox.h"
#include "../game/gameobject.h"
#include "../game/collision.h"
#include "../world/managegrid.h"
#include "../game/objecthelper.h"
#include "../world/voxeltraversal.h"
#include "../util/dynamicarray.h"
#include "../debugger/debug.h"
#include "playerinventory.h"
#include "../items/itemutil.h"
#include "../renderer/decal.h"
#include "../game/System.h"
#include "../util/cached.h"

// Picks the face of the block that the point lies closest to
inline Dir::Dir3d closest_face(v3::Vec3 pos, block* blk) {
    pos -= blk->center();
    Dir::Dir3d best = Dir::up3d;
    for (auto d : Dir::Directions3d) {
        if (dot(d.ToVec(), pos) > dot(best.ToVec(), pos)) best = d;
    }
    return best;
}

// Returns two orthogonal axes on the block face plane
inline util::pair<v3::Vec3, v3::Vec3> get_flat_frame(Dir::Dir3d dir) {
    switch (dir.ind() / 2) {
    case 0: return { {0,0,1}, {0,1,0} };
    case 1: return { {1,0,0}, {0,0,1} };
    case 2: return { {1,0,0}, {0,1,0} };
    default: throw std::logic_error("Invalid direction");
    }
}

void initbreakparticle(gameobject::obj newent);

struct playerbreak : gameobject::component {
    voxtra::WorldRayCollision closest;
    util::change <item*> pickaxe;

    decal break_decal;
    float break_start_time = 0.f;
    float timeuntilbreak = 0.f;
    util::change<block*> currmining;

    void start() override {
        currmining();
        currmining.reset(nullptr);
        CtxName::ctx.Ren->Textures.LoadTexture("images\\menutex.png", "MenuTexture");
    }

    void spawn_decal(size_t phase) {
        if (phase == 0) return;
        if (break_decal.handle) break_decal.destroy();
        static const char* tex[] = { "",
            "images\\block_break_1.png","images\\block_break_2.png",
            "images\\block_break_3.png","images\\block_break_4.png",
            "images\\block_break_5.png","images\\block_break_6.png" };
        std::string name = tex[phase];
        std::string handle = "block_break_" + std::to_string(phase);
        break_decal.create_handle(name.c_str(), handle.c_str());
        auto hit = closest.unwrap().Hit.intersectionpoint;
        Dir::Dir3d fd = closest_face(hit, currmining());
        auto& f = currmining()->mesh[fd.ind()];
        break_decal.center = f.center();
        break_decal.normal = fd.ToVec();
        auto [b, t] = get_flat_frame(fd);
        break_decal.bi_tangent = b * 0.5f;
        break_decal.tangent = t * 0.5f;
        break_decal.create_mesh();
        break_decal.render();
    }

    playerbreak() {};
    float block_power(block* blk) {
        return blk->mininglevel;

    }
    float curr_mining_power() {
        if (pickaxe()==nullptr)
        {
            return 1;
        }
        return pickaxe()->properties.pickaxepower;

    }
    void engage_block(block* blk) {
        if (!engaged())
        {

            pickaxe.reset(owner.getcomponent<inventory>().selected);
            currmining.reset( blk);
            break_start_time = block_power(blk);
            timeuntilbreak = break_start_time;
        }
        else
        {
            pickaxe.set(owner.getcomponent<inventory>().selected);
            currmining.set(blk);
        }
    }
    void disengage_block() {
        currmining.reset(nullptr);
        pickaxe.reset(nullptr);
        timeuntilbreak = -1;

    }
    bool engaged() {
        return currmining() != nullptr;
    }
    //returns current speed
    bool ensure_engage()  {
        ray r(owner.transform().position,
            owner.transform().position + owner.transform().getnormaldirection() * 7.f);
        closest = collision::raycastall(r, collision::HitQuery(owner), voxtra::countsolid);
        if (!closest) {
            return false;
        }
        auto hit = *closest;
        if (!hit.collider->owner.hascomponent<blockname::block>()) { 
            return false;
        }
        if (!inrange(hit.dist(), interactminrange, interactmaxrange)) {
            return false;
        }
        engage_block(hit.collider->owner.getcomponentptr<block>());
        
            if (engaged())
            {
                if (currmining.changed()|| pickaxe.changed()) {
                    return false;
                }
               
            }
            if (!CtxName::ctx.Inp->mouseleft().held) {

                return false;
            }
            if (currmining()->minedfastwithpick && curr_mining_power() < currmining()->mininglevel)
            {
                return false;
            }
           
            return true;
    }

    // Called each frame while holding the mouse;
    void try_modify() {
        if (ensure_engage())
        {
            
            timeuntilbreak -= curr_mining_power() * CtxName::ctx.Time->dt;
            // Show progress decal
            float prog = (break_start_time - timeuntilbreak) / break_start_time;
            size_t phase = clamp(size_t(prog * 7.f), 0, 6);
            if (apx(currmining()->mesh.box.scale, blockscale)) {
                spawn_decal(phase);
            }
            if (timeuntilbreak <= 0.f) {
                on_break(currmining());
             
            }
        }
        else
        {
            disengage_block();
        }
    }
       
    void on_break(block* broken) {
        
        // Break when timer completes
        if (timeuntilbreak <= 0.f) {
            if (pickaxe())
            {
                pickaxe()->use(1);
            }

            make_drop(broken->owner);
            gridutil::setblock(broken->pos, minecraftair);
            disengage_block();
        }
    }
    void update() {
        try_modify();
    }
    void make_drop(gameobject::obj Hit);

};

struct PlayerUpdateSystem : System {
    void run(gameobject::Ecs* ecs) override {
        for (auto [pb] : query::View<playerbreak>(ecs)) {
         

            pb->try_modify();
            

        };
    }
};
