#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../util/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "../game/entitystate.h"
#include "../items/menu.h"
#include "playerclimb.h"
#ifndef player_H
#define player_H
struct playermovement:gameobject::component
{
    Vector3 velocitylast;
    bool prevonground;
    void start() {

        prevonground = false;
        velocitylast = zerov;
        priority = 11;
    }float lastgroundy;
	virtual void update() {
       //objutil::toent(owner).transform.position += Vector3(0, 16, 0);
        Vector3& pos = objutil::toent(owner).transform.position;
        float slowdown = 2;
        Vector3& velocity = owner->getcomponent<rigidbody>().velocity;
        float speed = 16;
        float effectivespeed = (timename::smoothdt * speed);
        prevonground = owner->getcomponent<rigidbody>().isonground;

        if (userinput::getinputkey('s').held)
        {
            velocity -= v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw)))*effectivespeed*slowdown;
        }
        if (userinput::getinputkey('w').held)
        {
         velocity += v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * effectivespeed * slowdown;
        }
        if (userinput::getinputkey('a').held)
        {
           velocity += v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw))) * effectivespeed * slowdown;
        }
        if (userinput::getinputkey('d').held)
        {
            velocity-= v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw)))*effectivespeed * slowdown;
        }
        bool normalstate = true;
        if ( owner->getcomponent<rigidbody>().inliquid)
        {
            normalstate = false;
            if (userinput::getinputkey(shiftkey).held)
            {
                
                velocity.y = -10;
            }
            if (userinput::getinputkey(' ').held )
            {
                velocity.y = 10;
            }
        }
        if (owner->getcomponent<playerclimb>().onrope)
        {
            
            normalstate = false;
            if (userinput::getinputkey(shiftkey).held)
            {
                velocity.y = -5;
            }
            if (userinput::getinputkey(' ').held )
            {
                velocity.y = 5;
            }
        }
        if (normalstate)
        {

        
            if (userinput::getinputkey(' ').pressed && owner->getcomponent<rigidbody>().isonground)
            {
                velocity.y = 150 * speed / 200;
            }

            if (userinput::getinputkey(shiftkey).held)
            {
                velocity.y -= 1 * effectivespeed;
            }
        }
	}


};
#endif // !player_H
