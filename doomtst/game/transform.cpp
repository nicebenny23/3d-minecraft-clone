#pragma once
#include "transform.h"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp> #include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace v3;

Transform::Transform()
{
    yaw =90;
    pitch = 0;
	scale = v3::unit_scale;
}
Transform::Transform(v3::Point3 pos, float newyaw, float newpitch, v3::Scale3 newscale)
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

v3::Vec3 Transform::getnormaldirection()
{
    return yaw_pitch(yaw, pitch);
}

v3::Vec3 Transform::getrightdirection()
{
    return zero_fixed_normal(Cross(getnormaldirection(),v3::up));
}

v3::Vec3 Transform::getupdirection()
{
    return Cross(getrightdirection(),getnormaldirection());
}


void Transform::OrientDir(v3::Vec3 Direction)
{
    
    Direction = zero_fixed_normal(Direction);

    pitch = (glm::degrees(std::asin(Direction.y)));
    yaw = (glm::degrees(std::atan2(Direction.z, Direction.x)));

}
void Transform::Orient(v3::Point3 LookTowards)
{
    OrientDir(LookTowards-position);
}
glm::mat4 lookRotationMatrix(Vec3 vec) {
    
    Vec3 forward = vec;

    // Define a default up vector
    Vec3 WorldUp = up;
    if (Cross(forward, WorldUp)== zerov){
        // If forward vector is collinear with up vector, use a different up vector
        WorldUp= Vec3(1.0f, 0.0f, 0.0f);
    }

   
    Vec3 right = normal(Cross(WorldUp, forward));
    Vec3 trueUp = normal(Cross(forward, right));
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0] = glm::vec4( right.glm(), 0.0f);
    rotation[1] = glm::vec4(trueUp.glm(), 0.0f);
    rotation[2] = glm::vec4(forward.glm(), 0.0f);
    return rotation;
}

glm::mat4 LookAt(Transform& transform)
{

    return  glm::lookAt(transform.position.glm(), transform.position.glm() + transform.getnormaldirection().glm(), transform.getupdirection().glm());
}

glm::mat4 Transform::ToMatrix()
{
    glm::mat4 rotation = lookRotationMatrix(getnormaldirection());
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x,scale.y,scale.z));
    glm::mat4 translation = glm::translate(glm::mat4(1.0f), position.glm());

    // Combine all transformations (scale * rotation * translation)
    glm::mat4 modelMatrix = translation * rotation * scaleMat;
    return modelMatrix;
}


// Helper function to decompose a glm::mat4 into a Transform
Transform Decompose(const glm::mat4& matrix) {
    glm::vec3 scale,pos,skew;
    glm::quat quatRotation;
    glm::vec4 perspective;
    
    bool success= glm::decompose(matrix, scale, quatRotation, pos, skew, perspective);
    if (!success)
    {
        throw std::logic_error("decomposition did not succeed");
    }

    glm::vec3 eulerRotation = glm::eulerAngles(quatRotation);

    Transform transform;
    transform.position = Point3(pos.x,pos.y,pos.z);
    transform.yaw = glm::degrees(eulerRotation.y);
    transform.pitch = glm::degrees(eulerRotation.x);
    transform.scale = Scale3(scale.x,scale.y,scale.z);
    return transform;
}
// Function to combine two Transforms into a new Transform
Transform Compose(Transform& t1, Transform& t2) {
    glm::mat4 mat1 = (t1.ToMatrix());
    glm::mat4 mat2 = (t2.ToMatrix());
    glm::mat4 composed = mat2 * mat1;
    return Decompose(composed);
     
}