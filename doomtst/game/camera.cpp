
#include "camera.h"
#include "../util/vector3.h";
#include "../items/menu.h"
#include "../player/player.h"
#include "../player/cameracomp.h"
namespace camera {

	
	v3::Vec3 GetCamFront()
	{
		return GetCam().getnormaldirection();;
	}
	v3::Vec3 GetCamUp()
	{
		return GetCam().getupdirection();
	}
	v3::Vec3 GetCamRight()
	{
		return GetCam().getrightdirection();
	}
	Transform GetCam()
	{
		CameraComp* cam = player::goblin.getcomponentptr<CameraComp>();
		if (cam == nullptr) {
			throw std::logic_error("Attempted to access Camera before it was created");
		}
		return  cam->CamTransform;
	}
	v3::Vec3 campos() {

		return GetCam().position.glm();

	}
	

}