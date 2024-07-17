#include "util/vector3.h"
#include "game/gameobject.h"
#include "game/aabb.h"
#include "util/time.h"
#include "game/entity.h"
#include "game/objecthelper.h"

using namespace v3;

#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

struct rigidbody : gameobject::component {
    Vector3 velocity;
    Vector3 unsetpositon;
    Vector3 acceleration;
    float mass = 1.0f;
    float restitution = 0.1f;
    aabb::Collider* boundingbox;

    // Constructor
    rigidbody() : velocity(zerov), unsetpositon(zerov), acceleration(zerov), boundingbox(nullptr) {}

    // Start function to initialize the rigidbody component
    void start() {
        acceleration = zerov;
        unsetpositon = objutil::toent(owner).pos;
        boundingbox = &owner->getcomponent<aabb::Collider>();
    }

    // Update function called every frame
    void update() {
        applyGravity();
        integrate();
        unsetpositon = objutil::toent(owner).pos;
    }

    // Apply gravity to the rigidbody
    void applyGravity() {
        Vector3 gravity(0, -40.81f, 0);
        acceleration += gravity / mass;
    }

    // Integrate the acceleration to update velocity and position
    void integrate() {
        float deltaTime = timename::mindt;
        velocity += acceleration * deltaTime;
        velocity *= 0.98f;  // Adjust damping factor to prevent excessive damping
        objutil::toent(owner).pos += velocity * deltaTime;
        acceleration = Vector3(0, 0, 0);  // Reset acceleration after integration
    }

    // Compute collision with another collision rectangle
    void computecollision(aabb::Collider* Collider, Vector3 force) {
       
    }
};

#endif // RIGIDBODY_HPP