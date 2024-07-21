#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../util/time.h"
#include "../game/rigidbody.h"
#include "../world/voxeltraversal.h"
#include "../game/entitystate.h"
#include "../items/menu.h"
#ifndef player_H
#define player_H
struct playermovement:gameobject::component
{
    Vector3 velocitylast;
    bool prevonground;
    void start() {

        prevonground = false;
        velocitylast = zerov;
        priority = -1111;
    }float lastgroundy;
	virtual void update() {

        Vector3& pos = objutil::toent(owner).transform.position;
            
        Vector3& velocity = owner->getcomponent<rigidbody>().velocity;
        float speed = 16;
        float effectivespeed = (timename::smoothdt * speed);
        prevonground = owner->getcomponent<rigidbody>().isonground;

        if (userinput::getinputkey('s').held)
        {
            pos -= v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw)))*effectivespeed;
        }
        if (userinput::getinputkey('w').held)
        {
         pos += v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * effectivespeed;
        }
        if (userinput::getinputkey('a').held)
        {
           pos += v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw))) * effectivespeed;
        }
        if (userinput::getinputkey('d').held)
        {
            pos -= v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw)))*effectivespeed;
        }
        if (userinput::getinputkey(' ').pressed&&owner->getcomponent<rigidbody>().isonground)
        {
           velocity.y = 250 * effectivespeed;
        }
        if (userinput::getinputkey('z').held)
        {
            velocity.y -= 1 * effectivespeed;
        }

	}


};
#endif // !player_H
