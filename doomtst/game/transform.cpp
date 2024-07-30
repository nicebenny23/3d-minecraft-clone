#include "transform.h"
#include <glm/gtx/matrix_decompose.hpp>
using namespace v3;
Transform::Transform()
{
    yaw = 0;
    pitch = 0;
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


void Transform::orientbase(v3::Vector3 base)
{
    base= normal(base);
    float newpitch = -std::asin(base.y);
    float newyaw = std::atan2(base.z, base.x);
    pitch = glm::degrees(newpitch);
    yaw = glm::degrees(newyaw);
}
void Transform::orient(v3::Vector3 to)
{
    to -= position;

    orientbase(to);
}
// Convert a Transform to a glm::mat4
glm::mat4 transformtomat( Transform& transform) {
    glm::mat4 trans = glm::mat4(1.0f);

    // Apply translation

    trans = glm::translate(trans, transform.position.glm());
   
    trans = glm::rotate(trans, glm::radians(-transform.yaw), glm::vec3(0, 1, 0));
    // Apply pitch (rotation around the x-axis)
    trans = glm::rotate(trans, glm::radians(-transform.pitch), glm::vec3(0, 0, 1));
    // Apply yaw (rotation around the y-axis) 

   
 
    // Apply 
    trans = glm::scale(trans, transform.scale.glm());
    return trans;
}

// Helper function to decompose a glm::mat4 into a Transform
// Decompose a glm::mat4 into a Transform
Transform decompose(const glm::mat4& matrix) {
    Transform transform;
    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;

    // Decompose the matrix
    if (glm::decompose(matrix, scale, rotation, translation, skew, perspective)) {
        // Set the translation and scale
        transform.position = v3::Vector3(translation.x, translation.y, translation.z);
        transform.scale = v3::Vector3(scale.x, scale.y, scale.z);
    };

        // Convert rotation quaternion to Euler angles
        glm::vec3 eulerAngles = glm::eulerAngles(rotation);
        transform.pitch = glm::degrees(eulerAngles.x);
        transform.yaw = glm::degrees(eulerAngles.y);

   

    return transform;
}

// Function to combine two Transforms into a new Transform
Transform compose(Transform& t1, Transform& t2) {
    glm::mat4 mat1 = transformtomat(t1);
    glm::mat4 mat2 = transformtomat(t2);
    glm::mat4 composed = mat2 * mat1;
    return decompose(composed);
}
glm::mat4 tcompose(Transform& t1, Transform& t2) {
    glm::mat4 mat1 = transformtomat(t1);
    glm::mat4 mat2 = transformtomat(t2);
    glm::mat4 composed = mat2 * mat1;
    return composed;
}
