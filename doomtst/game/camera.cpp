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
		Vector3 offset = player::goblin->transform.getnormaldirection();
		offset.y = 0;
		offset = normal(offset)/2;
		offset += Vector3(0, 1, 0);
		offset /= 2;
		campos = player::goblin->transform.position.glm() + offset.glm();
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
		frontvec	= player::goblin.toent()->transform.getnormaldirection().glm();
		rightvec=  normal(crossprod(frontvec, Vector3(0, 1, 0)));;
upvec= normal(crossprod(rightvec, frontvec));
		glm::vec3 cameraFront = frontvec.glm();
		glm::vec3 Right = rightvec.glm();  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			glm::vec3 Up = upvec.glm();
		
		renderer::setviewmatrix(glm::lookAt(campos, campos + cameraFront, Up));
		
	
	}

	
	void cameraupdate()
	{
		player::calculateyawandpitch();
		updatetoplayer();


		sendoffviewmatrix();



	}
}