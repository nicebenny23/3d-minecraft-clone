#include "transform.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <cmath>
using namespace v3;
Transform::Transform()
{
    yaw =90;
    pitch = 0;
	scale = v3::unitv ;
}

Transform::Transform(v3::Vector3 pos, float newyaw, float newpitch, v3::Vector3 newscale)
{
	position = pos;
	yaw = newyaw;
	pitch = newpitch;
	scale = newscale;
    glm::mat4 a = transformtomat(*this);
    int l = 1;
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

v3::Vector3 Transform::getrightdirection()
{
    return normal(crossprod(getnormaldirection(),v3::up));
}

v3::Vector3 Transform::getupdirection()
{
    return crossprod(getrightdirection(),getnormaldirection());
}


void Transform::orientbase(v3::Vector3 base)
{
    base = normal(base);

    pitch = (glm::degrees(std::asin(base.y)));
    yaw = (glm::degrees(std::atan2(base.z, base.x)));

}
void Transform::orient(v3::Vector3 to)
{
    orientbase(to-position);
}
glm::mat4 rotationmatfromvec(Vector3 vec) {
    
    glm::vec3 forward = (vec).glm();

    // Define a default up vector
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    if (glm::length(glm::cross(forward, up)) < 0.0001f) {
        // If forward vector is collinear with up vector, use a different up vector
        up = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    // Calculate the right vector
    glm::vec3 right = normalize(glm::cross(up, forward));
    // Recompute the true up vector
    glm::vec3 trueUp = normalize(glm::cross(forward, right));

    // Create rotation matrix
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0] = glm::vec4(right, 0.0f);
    rotation[1] = glm::vec4(trueUp, 0.0f);
    rotation[2] = glm::vec4(forward, 0.0f);
    return rotation;
}
// Convert a Transform to a glm::mat4
glm::mat4 transformtomat( Transform& transform) {
   
    //set to face foward from not
   
  glm::mat4 rotation =rotationmatfromvec( transform.getnormaldirection());

  glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), transform.scale.glm());

    // Create translation matrix
  glm::mat4 translation = glm::translate(glm::mat4(1.0f), transform.position.glm());

    // Combine all transformations (scale * rotation * translation)
    glm::mat4 modelMatrix = translation *rotation* scaleMat;
    return modelMatrix;
}

// Helper function to decompose a glm::mat4 into a Transform
void decompose(const glm::mat4& matrix, Transform& transform) {
    glm::vec3 scale;
    glm::vec3 pos;
    glm::quat quatRotation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(matrix, scale, quatRotation, pos, skew, perspective);

    transform.position = Vector3(pos);
    glm::vec3 eulerRotation = glm::eulerAngles(quatRotation);

    transform.yaw = glm::degrees(eulerRotation.y);
    transform.pitch = glm::degrees(eulerRotation.x);
    transform.scale = Vector3(scale);
}
// Function to combine two Transforms into a new Transform
Transform compose(Transform& t1, Transform& t2) {
    glm::mat4 mat1 = transformtomat(t1);
    glm::mat4 mat2 = transformtomat(t2);
    glm::mat4 composed = mat2 * mat1;
    Transform* newtrat = new Transform();
     decompose(composed,*newtrat);
     return *newtrat;
}
glm::mat4 tcompose(Transform& t1, Transform& t2) {
    glm::mat4 mat1 = transformtomat(t1);
    glm::mat4 mat2 = transformtomat(t2);
    glm::mat4 composed = mat2 * mat1;
    return composed;
}
