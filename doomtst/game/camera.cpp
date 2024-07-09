#include "camera.h"
#include "../util/vector3.h";

namespace camera {
	glm::vec3 campos;
	float yaw, pitch;
	void initilize()
	{
		campos = glm::vec3(0,16,-1);
		yaw = -90;
		pitch = 0;

	}
    v3::Vector3 direction() {

		v3::Vector3 dir = v3::zerov;
		 dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		dir.y = sin(glm::radians(pitch));
		dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	return v3::normal(dir);
	}
	
	void calculateyawandpitch()
	{
		

			float xoffset = -userinput::mouseposdt.x;
			float yoffset = userinput::mouseposdt.y;

			float sensitivity = 0.3;
			
			if (abs(xoffset) > 180)
			{
				xoffset = 0;
			}  if (abs(yoffset) > 180)
			{
				yoffset = 0;
			}
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
	}
	
	void sendoffviewmatrix()
	
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		glm::vec3 cameraFront = glm::normalize(direction);
		glm::vec3 Right = glm::normalize(glm::cross(cameraFront, glm::vec3(0, 1, 0)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		glm::vec3    Up = glm::normalize(glm::cross(Right, cameraFront));
		renderer::setviewmatrix(glm::lookAt(campos, campos + cameraFront, Up));

	}

	void setcamerapos(v3::Vector3 newpos)
	{
		campos = newpos.glm();
	}
}