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
	void orient(v3::Vector3 to);
	void orientbase(v3::Vector3 base);
};
glm::mat4 transformtomat(Transform& transform);
glm::mat4 tcompose(Transform& t1, Transform& t2);
Transform compose(Transform& t1, Transform& t2);

#endif // ! transform_HPP
