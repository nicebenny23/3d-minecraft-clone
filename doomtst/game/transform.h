#include "../util/vector3.h"
#include <GLFW/glfw3.h>
#ifndef  transform_HPP
#define transform_HPP
struct Transform
{
	Transform();
	Transform(v3::Vector3 pos, float newyaw, float newpitch, v3::Vector3 newscale);
	v3::Vector3 position;
	float yaw;
	float pitch;
	v3::Vector3 scale;
	void rotate(float ptch,float yw);
	v3::Vector3 getnormaldirection();
	void orient(v3::Vector3 to);
};


#endif // ! transform_HPP
