#ifndef Camera_HPP
#define Camera_HPP
#include "userinput.h"

#include "renderer.h"
namespace camera {
	extern float yaw, pitch;
	extern glm::vec3 campos;
	void initilize();
	void calculateyawandpitch();
	void sendoffviewmatrix();
	void setcamerapos(glm::vec3 newpos);
}

#endif // !Camera_HPP
