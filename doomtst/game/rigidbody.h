#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../game/aabb.h"
#include "../util/time.h"
#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/geometry.h"
#include "../world/voxeltraversal.h"
#include "System.h"

using namespace v3;
#pragma once 
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
        Vector3 boxcenter = owner.transform().position - Vector3(0, boundingbox->globalbox().scale.y + .05, 0);
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
        unsetpositon = owner.transform().position;
        boundingbox = &owner.getcomponent<aabb::Collider>();  
        priority = -111;
    }


    // Apply gravity to the rigidbody
    void applyGravity() {
        
        Vector3 gravity(0, -9.81f, 0);
        
        acceleration += gravity*gravityscale / mass;
    }

    // Integrate the acceleration to update velocity and position
    void integrate() {
        float deltaTime = CtxName::ctx.Time->dt;
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

        owner.transform().position += velocity * deltaTime;
        acceleration = Vector3(0, 0, 0);  // Reset acceleration after integration
    }

  
};
struct RigidbodySystem :System {

    void run(gameobject::Ecs* ecs) override {
        int l = 6;
        query::View<rigidbody, gameobject::transform_comp> rigids(ecs);
        for (auto [body, pos] : rigids) {
            body->calculateonground();
            body->applyGravity();
            body->integrate();
            body->unsetpositon = pos->transform.position;
        }

    }

};
struct RigidbodySystem :System {

    void run(gameobject::Ecs* ecs) override {
        int l = 6;
        query::View<rigidbody, gameobject::transform_comp> rigids(ecs);
        for (auto [body, pos] : rigids) {
            body->calculateonground();
            body->applyGravity();
            body->integrate();
            body->unsetpositon = pos->transform.position;
        }

    }

};