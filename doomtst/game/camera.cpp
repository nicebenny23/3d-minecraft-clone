#include "camera.h"
#include "../util/vector3.h";
#include "../items/menu.h"
#include "../player/player.h"
#include "../renderer/Window.h"
namespace camera {
	glm::vec3 campos;
	float yaw, pitch;
	v3::Vector3 upvec;
	v3::Vector3 frontvec;
	v3::Vector3 rightvec;
	void initilize()
	{
		Vector3 playerpos=player::goblin->transform.position;
		campos = playerpos.glm();
		yaw = -90;
		pitch = 0;

	}
	void updatetoplayer()
	{
		campos = player::goblin->transform.position.glm();
		pitch = player::goblin->transform.pitch;
		yaw= player::goblin->transform.yaw;
	}
    v3::Vector3 direction() {

		v3::Vector3 dir = v3::zerov;
		 dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		dir.y = sin(glm::radians(pitch));
		dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	return v3::normal(dir);
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
		upvec = v3::Vector3(Up);
		rightvec= v3::Vector3(Right);
		frontvec= v3::Vector3(cameraFront);

		renderer::setviewmatrix(glm::lookAt(campos, campos + cameraFront, Up));
		
	
	}

	void setcamerapos(v3::Vector3 newpos)
	{
		campos = newpos.glm();
	}
	
	void cameraupdate()
	{
		player::calculateyawandpitch();
		updatetoplayer();


		sendoffviewmatrix();



	}
}