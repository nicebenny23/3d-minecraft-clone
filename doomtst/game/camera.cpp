
#include "camera.h"
#include "../math/vector3.h";
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
		CameraComp* cam = player::goblin.get_component_ptr<CameraComp>();
		if (cam == nullptr) {
			throw std::logic_error("Attempted to access Camera before it was created");
		}
		if (isnan(cam->CamTransform.position.x))
		{
			int l = 2;
		}
		return  cam->CamTransform;
	}
	v3::Point3 campos() {

		
		return GetCam().position;

	}
	

}