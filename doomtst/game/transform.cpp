#include "transform.h"

Transform::Transform()
{
	scale = v3::unitv / 2;
}

Transform::Transform(v3::Vector3 pos, float newyaw, float newpitch, v3::Vector3 newscale)
{
	position = pos;
	yaw = newyaw;
	pitch = newpitch;
	scale = newscale;
}

void Transform::rotate(float ptch, float yw)
{
	pitch += ptch;
	yaw += yw;
}

v3::Vector3 Transform::getnormaldirection()
{
	v3::Vector3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return v3::normal(direction);
}

void Transform::orient(v3::Vector3 to)
{
	v3::Vector3 up=v3::Vector3(0, 1, 0);  // Assuming up vector is Y-axis
	to -= position;
	float newpitch = std::asin(to.y);
	float newyaw = std::atan2(to.z, to.x);
	pitch = newpitch;
	yaw = newyaw;
}
