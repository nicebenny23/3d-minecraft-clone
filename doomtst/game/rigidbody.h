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
    Vec3 velocity;
    Vec3 unsetpositon;
    Vec3 acceleration;
    float mass = 1.0f;
    aabb::Collider* boundingbox;
    bool isonground;
    bool inliquid;
    float  gravityscale;
    float friction;
    void calculateonground() {

        inliquid = false;
        Vec3 boxcenter = owner.transform().position - Vec3(0, boundingbox->globalbox().scale.y + .05, 0);
        geometry::Box checkbox = geometry::Box(boxcenter, Vec3(boundingbox->globalbox().scale.x, .1, boundingbox->globalbox().scale.z) * .999);
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
        
        Vec3 gravity(0, -9.81f, 0);
        
        acceleration += gravity*gravityscale / mass;
    }

    // Integrate the acceleration to update velocity and position
    //should definitly clamp
    void integrate() {
        float deltaTime = CtxName::ctx.Time->dt;
        velocity += acceleration * deltaTime;
        if (isonground)
        {
            velocity.y *= clamp(1 - deltaTime * 10, 0.0, 1.0);
        }
        if ((velocity.y)>10000)
        {
            int l = 1;
        }
        if (gravityscale!=0)
        {

            velocity.y *= clamp(1 - deltaTime*.1f,0.0,1.0);

        }velocity.x *= clamp(1 - deltaTime*friction, 0.0, 1.0);  // Adjust damping factor to prevent excessive damping

        velocity.z *= clamp(1 - deltaTime * friction, 0.0, 1.0);  // Adjust damping factor to prevent excessive damping

        owner.transform().position += velocity * deltaTime;
        acceleration = Vec3(0, 0, 0);  // Reset acceleration after integration
    }

  
};
struct RigidbodySystem :System {

    void run(gameobject::Ecs* ecs) override {

ecs->arch.check();
        int l = 6;
         

        query::View< gameobject::transform_comp, rigidbody> rigids(ecs);
        for (auto [pos,body] : rigids) {
            body->calculateonground();
            body->applyGravity();
            body->integrate();
      
            body->unsetpositon = pos->transform.position;
         //   pos->transform.position = Coord(0, 0, 0);
        }

    }

};
