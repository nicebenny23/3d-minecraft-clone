#pragma once
#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../util/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "../game/System.h"
#include "../game/entitystate.h"
#include "playerclimb.h"

struct playermovement: gameobject::component
{
  
    void start() {
        priority = 11;
    }float lastgroundy;
	

};

struct PlayerMovementSys:System {
    using Dependencies = TypeList::TypeList<RigidbodySystem>;
    virtual void run(gameobject::Ecs* ecs) {

        
        query::View<rigidbody, playermovement, playerclimb> rigids(ecs);
        for (auto [body, movment,climb] : rigids) {
          
            float slowdown = 2;
            float speed = 16;
            float effectivespeed = (CtxName::ctx.Time->dt * speed) * slowdown;
            Vec3 fv = normal(Vec3(camera::GetCamFront().x, 0, camera::GetCamFront().z));
            Vec3 rv = normal(Vec3(camera::GetCamRight().x, 0, camera::GetCamRight().z));

            if (CtxName::ctx.Inp->getKey('s').held)
            {
                body->velocity -= fv * effectivespeed;
            }
            if (CtxName::ctx.Inp->getKey('w').held)
            {
                body->velocity += fv * effectivespeed;
            }
            if (CtxName::ctx.Inp->getKey('a').held)
            {
                body->velocity -= rv * effectivespeed;
            }
            if (CtxName::ctx.Inp->getKey('d').held)
            {
                body->velocity += rv * effectivespeed;
            }
  
            bool normalstate = true;
            if (body->inliquid)
            {
                normalstate = false;
                if (CtxName::ctx.Inp->getKey(shiftkey).held)
                {

                    body->velocity.y = -10;
                }
                if (CtxName::ctx.Inp->getKey(' ').held)
                {
                    body->velocity.y = 10;
                }

            }
            if (climb->onrope)
            {

                normalstate = false;
                if (CtxName::ctx.Inp->getKey(shiftkey).held)
                {
                    body->velocity.y = -5;
                }
                if (CtxName::ctx.Inp->getKey(' ').held)
                {
                    body->velocity.y = 5;
                }
            }
            if (normalstate)
            {


                if (CtxName::ctx.Inp->getKey(' ').pressed && body->isonground)
                {
                    body->velocity.y = 150 * speed / 200;
                }

                if (CtxName::ctx.Inp->getKey(shiftkey).held)
                {
                    body->velocity.y -= 1 * effectivespeed;
                }
            }
        }
        }
    

};
