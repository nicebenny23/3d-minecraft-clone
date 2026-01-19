#pragma once 
#include "../util/userinput.h"
#include "../math/transform.h"
#include "../math/vector3.h"
#include "../renderer/renderer.h"
namespace camera {
	
	v3::Vec3 GetCamFront();
	v3::Vec3 GetCamUp();
	v3::Vec3 GetCamRight();
	math::Transform GetCam();
v3::Point3 campos();

}

 // !Camera_HPP
