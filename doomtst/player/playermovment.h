#pragma once
#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../util/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "../game/entitystate.h"
#include "playerclimb.h"
struct playermovement: gameobject::component
{
    Vector3 velocitylast;
    bool prevonground;
    void start() {

        prevonground = false;
        velocitylast = zerov;
        priority = 11;
    }float lastgroundy;
	virtual void update() {
       //owner.transform.position += Vector3(0, 16, 0);
        Vector3& pos = owner.transform().position;
        float slowdown = 2;
        Vector3& velocity = owner.getcomponent<rigidbody>().velocity;
        float speed = 16;
        float effectivespeed = (CtxName::ctx.Time->dt * speed) * slowdown;
        Vector3 fv =normal( Vector3(camera::GetCamFront().x, 0, camera::GetCamFront().z));
        Vector3 rv =normal( Vector3(camera::GetCamRight().x, 0, camera::GetCamRight().z));
   
        if (CtxName::ctx.Inp->getKey('s').held)
        {
            velocity -=  fv*effectivespeed;
        }
        if (CtxName::ctx.Inp->getKey('w').held)
        {
         velocity += fv * effectivespeed;
        }
        if (CtxName::ctx.Inp->getKey('a').held)
        {
           velocity -= rv * effectivespeed ;
        }
        if (CtxName::ctx.Inp->getKey('d').held)
        {
            velocity+=rv *effectivespeed ;
        }
        prevonground = owner.getcomponent<rigidbody>().isonground;

        bool normalstate = true;
        if ( owner.getcomponent<rigidbody>().inliquid)
        {
            normalstate = false;
            if (CtxName::ctx.Inp->getKey(shiftkey).held)
            {
                
                velocity.y = -10;
            }
            if (CtxName::ctx.Inp->getKey(' ').held )
            {
                velocity.y = 10;
            }

        }
        if (owner.getcomponent<playerclimb>().onrope)
        {
            
            normalstate = false;
            if (CtxName::ctx.Inp->getKey(shiftkey).held)
            {
                velocity.y = -5;
            }
            if (CtxName::ctx.Inp->getKey(' ').held )
            {
                velocity.y = 5;
            }
        }
        if (normalstate)
        {

        
            if (CtxName::ctx.Inp->getKey(' ').pressed && owner.getcomponent<rigidbody>().isonground)
            {
                velocity.y = 150 * speed / 200;
            }

            if (CtxName::ctx.Inp->getKey(shiftkey).held)
            {
                velocity.y -= 1 * effectivespeed;
            }
        }
	}


};