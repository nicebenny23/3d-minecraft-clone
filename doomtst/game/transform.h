#pragma once
#include "../util/vector3.h"
#include <glm/mat4x4.hpp>

struct Transform
{
	Transform();
	Transform(v3::Vec3 pos, float newyaw, float newpitch, v3::Vec3 newscale);
	v3::Vec3 position;

	float yaw;
	float pitch;
	v3::Vec3 scale;
	void rotate(float ptch,float yw);
	v3::Vec3 getnormaldirection();
	v3::Vec3 getrightdirection();
	v3::Vec3 getupdirection();
	void Orient(v3::Vec3 to);
	void OrientDir(v3::Vec3 base);
	glm::mat4 ToMatrix();

	
};
glm::mat4 LookAt(Transform& transform);
Transform Compose(Transform& t1, Transform& t2);
Transform Decompose(const glm::mat4& matrix);
