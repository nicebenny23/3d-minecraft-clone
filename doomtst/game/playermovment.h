#include "game/entity.h"
#include "game/objecthelper.h"
#include "util/userinput.h"
#include <GLFW/glfw3.h>
#include "util/time.h"
#ifndef player_H
#define player_H
struct playermovement:gameobject::component
{
	
	virtual void update() {
        Vector3& pos = objutil::toent(owner).pos;
     
        float speed = 10;
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
        if (userinput::getinputkey(' ').held)
        {
           pos += v3::Vector3(0, 1, 0) * (timename::mindt * speed);
        }
        if (userinput::getinputkey('z').held)
        {
          pos -= v3::Vector3(0, 1, 0) * (timename::mindt * speed);
        }
     


	}


};
#endif // !player_H
