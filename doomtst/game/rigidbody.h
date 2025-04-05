#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../game/aabb.h"
#include "../util/time.h"
#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/geometry.h"
#include "../world/voxeltraversal.h"

using namespace v3;

#ifndef RIGIDBODY_HPP
#define RIGIDBODY_HPP

struct rigidbody : gameobject::component {
    Vector3 velocity;
    Vector3 unsetpositon;
    Vector3 acceleration;
    float mass = 1.0f;
    aabb::Collider* boundingbox;
    bool isonground;
    bool inliquid;
    float  gravityscale;
    float friction;
    void calculateonground() {

        inliquid = false;
        Vector3 boxcenter = objutil::toent(owner).transform.position - Vector3(0, boundingbox->globalbox().scale.y + .05, 0);
        geometry::Box checkbox = geometry::Box(boxcenter, Vector3(boundingbox->globalbox().scale.x, .1, boundingbox->globalbox().scale.z) * .999);
        isonground = (voxtra::Boxcollwithgrid(checkbox ));
    }
    // Constructor
    rigidbody() : velocity(zerov), unsetpositon(zerov), acceleration(zerov), boundingbox(nullptr) {
        friction = 1;
        gravityscale =1;
    } rigidbody(float fric,float gravenabled=1  ) : velocity(zerov), unsetpositon(zerov), acceleration(zerov), boundingbox(nullptr) {
        gravityscale = 1;
        friction = fric;
    }
    ~rigidbody() = default;
    // Start function to initialize the rigidbody component
    void start() {
        acceleration = zerov;
        unsetpositon = objutil::toent(owner).transform.position;
        boundingbox = &owner->getcomponent<aabb::Collider>();  
        priority = -111;
    }

    // Update function called every frame
    void update() {
        calculateonground();
    
            applyGravity();

        integrate();
        unsetpositon = objutil::toent(owner).transform.position;
    }

    // Apply gravity to the rigidbody
    void applyGravity() {
        
        Vector3 gravity(0, -9.81f, 0);
        
        acceleration += gravity*gravityscale / mass;
    }

    // Integrate the acceleration to update velocity and position
    void integrate() {
        float deltaTime = timename::dt;
        velocity += acceleration * deltaTime;
        if (isonground)
        {
            velocity.y *= 1-deltaTime*10;
        }
        if (gravityscale!=0)
        {

            velocity.y *= 1 - deltaTime;

        }velocity.x *= (1-deltaTime*friction);  // Adjust damping factor to prevent excessive damping

        velocity.z *= (1 - deltaTime * friction);  // Adjust damping factor to prevent excessive damping

        objutil::toent(owner).transform.position += velocity * deltaTime;
        acceleration = Vector3(0, 0, 0);  // Reset acceleration after integration
    }

  
};

#endif // RIGIDBODY_HPP