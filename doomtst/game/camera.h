#ifndef Camera_HPP
#define Camera_HPP
#include "../util/userinput.h"
#include "../util/vector3.h"
#include "../renderer/renderer.h"
namespace camera {
	extern float yaw, pitch;
	extern v3::Vector3 frontvec;
	extern v3::Vector3 upvec;
	extern v3::Vector3 rightvec;
	extern glm::vec3 campos;
	v3::Vector3 direction();
	void initilize();
	void updatetoplayer();
	void sendoffviewmatrix();
	void cameraupdate();
}

#endif // !Camera_HPP
