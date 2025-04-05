#include "camera.h"
#include "../util/vector3.h";
#include "../items/menu.h"
#include "../player/player.h"
#include "../renderer/Window.h"
namespace camera {
	v3::Vector3 campos;
	float yaw, pitch;
	v3::Vector3 upvec;
	v3::Vector3 frontvec;
	v3::Vector3 rightvec;
	void initilize()
	{
		campos = player::goblin->transform.position;
		yaw = -90;
		pitch = 0;

	}
	void updatetoplayer()
	{
	
		
	
		campos = player::goblin->transform.position.glm() ;
		pitch = player::goblin->transform.pitch;
		yaw= player::goblin->transform.yaw;
	}
    v3::Vector3 direction() {
		return v3::YawPitch(yaw, pitch);
	}
	
	
	void setviewmatrix()
	
	{
		frontvec = player::goblin.toent()->transform.getnormaldirection();
		rightvec = player::goblin.toent()->transform.getrightdirection();
		upvec = player::goblin.toent()->transform.getupdirection();
		renderer::setviewmatrix(glm::lookAt(campos.glm(), campos.glm() + frontvec.glm(), upvec.glm()));
		
	
	}

	
	void cameraupdate()
	{
		player::calculateyawandpitch();
		updatetoplayer();


		setviewmatrix();



	}
}