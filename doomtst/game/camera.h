#pragma once 
#include "../util/userinput.h"
#include "../game/transform.h"
#include "../util/vector3.h"
#include "../renderer/renderer.h"
namespace camera {
	
	v3::Vector3 GetCamFront();
	v3::Vector3 GetCamUp();
	v3::Vector3 GetCamRight();
	Transform GetCam();
v3::Vector3 campos();

}

 // !Camera_HPP
