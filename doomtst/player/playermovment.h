#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/userinput.h"
#include <GLFW/glfw3.h>
#include "../util/time.h"
#include "../rigidbody.h"
#include "../world/voxeltraversal.h"
#include "../game/entitystate.h"
#ifndef player_H
#define player_H
struct playermovement:gameobject::component
{
    float lastgroundy;
	virtual void update() {


        Vector3 aabbscale = owner->getcomponent<colrect>().scale;
        Vector3& pos = objutil::toent(owner).pos;
        Vector3 boxcenter = pos  - Vector3(0, aabbscale.y+.05 , 0) ;
        geometry::Box checkbox = geometry::Box(boxcenter, Vector3(aabbscale.x, .1, aabbscale.z)*unitscale);
        bool onground = (voxtra::Boxcollwithgrid(checkbox,100));
        Vector3& velocity = owner->getcomponent<rigidbody>().velocity;
        if (onground)
        {
            if (lastgroundy- pos.y>1)
            {
                objutil::toent(owner).getcomponent<estate>().damage(floor(abs(velocity.y) / 4.f));
                int l = 1;

                lastgroundy = pos.y;
            }
             
        }

        if (onground)
        {
          
        }
        
        float speed = 16;
        if (userinput::getinputkey('s').held)
        {
            pos -= v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * (timename::mindt * speed);
        }
        if (userinput::getinputkey('w').held)
        {
         pos += v3::Vector3(cos(glm::radians(camera::yaw)), 0, sin(glm::radians(camera::yaw))) * (timename::mindt * speed);
        }
        if (userinput::getinputkey('a').held)
        {
           pos += v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw))) * (timename::mindt * speed);
        }
        if (userinput::getinputkey('d').held)
        {
            pos -= v3::Vector3(sin(glm::radians(camera::yaw)), 0, -cos(glm::radians(camera::yaw))) * (timename::mindt * speed);
        }
        if (userinput::getinputkey(' ').pressed&&onground)
        {
           velocity.y = 100 * (timename::mindt * speed);
        }
        if (userinput::getinputkey('z').held)
        {
          pos -= v3::Vector3(0, 1, 0) * (timename::mindt * speed);
        }
       


	}


};
#endif // !player_H
