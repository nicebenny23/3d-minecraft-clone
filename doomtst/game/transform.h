#include "../util/vector3.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
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
	v3::Vector3 getrightdirection();
	v3::Vector3 getupdirection();
	void Orient(v3::Vector3 to);
	void OrientDir(v3::Vector3 base);
	glm::mat4 ToMatrix();
	
};
glm::mat4 LookAt(Transform& transform);
Transform Compose(Transform& t1, Transform& t2);
Transform Decompose(const glm::mat4& matrix);

#endif // ! transform_HPP
