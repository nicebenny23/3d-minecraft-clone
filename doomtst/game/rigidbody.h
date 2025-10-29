#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../game/aabb.h"
#include "../util/time.h"
#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../util/geometry.h"
#include "../world/voxeltraversal.h"
#include "System.h"
#include "../util/pipeline.h"
#include "../util/tag.h"
#include "../game/collision.h"
using namespace v3;
#pragma once 

struct rigid_force {
    Vec3 force;
    tag::Tag tag;
    stn::Option<timename::time> end_time;
    bool has_ended() {
        if (!end_time)
        {
            return true;
        }
        bool val= CtxName::ctx.Time->now()<end_time();
        return val;
    }
    rigid_force(Vec3 frc, float duration,const tag::Tag& tg):force(frc), end_time(CtxName::ctx.Time->now()+duration), tag(tg) {
        
    }
    rigid_force(Vec3 frc, float duration, const std::string& tg) :force(frc), end_time(CtxName::ctx.Time->now() + duration), tag(tg) {

    }
    rigid_force(Vec3 frc, const tag::Tag& tg) :force(frc), end_time(stn::None),tag(tg){

    }
    rigid_force(Vec3 frc, const std::string& tg) :force(frc), end_time(stn::None), tag(tg) {

    }
    rigid_force():end_time(stn::None),force(zerov) {


    }
};
struct rigidbody : gameobject::component {
    Vec3 velocity;
    Vec3 oldvelocity;
    Vec3 acceleration;
    float mass = 1.0f;
    array<rigid_force> forces;
    aabb::Collider* boundingbox;
    bool isonground;
    bool isonceil;
    bool inliquid;
    float  gravityscale;
    float friction;
    void calculateonground() {

        inliquid = false;
        Vec3 boxcenter = owner.transform().position - Vec3(0, boundingbox->globalbox().scale.y + .01, 0);
        geometry::Box checkbox = geometry::Box(boxcenter, Vec3(boundingbox->globalbox().scale.x, .005, boundingbox->globalbox().scale.z*.9f) * .92);
        isonground = (voxtra::Boxcollwithgrid(checkbox ));
    }
    void calculateonceil() {
        Vec3 boxcenter = owner.transform().position + Vec3(0, boundingbox->globalbox().scale.y + .01, 0);
        geometry::Box checkbox = geometry::Box(boxcenter, Vec3(boundingbox->globalbox().scale.x, .005, boundingbox->globalbox().scale.z) * .9);
        isonceil = (voxtra::Boxcollwithgrid(checkbox));
        if (isonceil)
        {
            int l = 2;
        }

    }
    // Constructor
    rigidbody() : velocity(zerov), oldvelocity(zerov), acceleration(zerov), boundingbox(nullptr) {
        friction = 1;
        gravityscale =1;
    } rigidbody(float fric,float gravenabled=1  ) : velocity(zerov), oldvelocity(zerov), acceleration(zerov), boundingbox(nullptr) {
        gravityscale = 1;
        friction = fric;

    }
    ~rigidbody() = default;
    // Start function to initialize the rigidbody component
    void start() {
        forces.push(rigid_force(v3::Vec3(0,-10,0), "gravity"));
        acceleration = zerov;
        oldvelocity = owner.transform().position;
        boundingbox = &owner.getcomponent<aabb::Collider>();  
        priority = -111;
    }


    void add_force(const rigid_force& frc) {
        forces.push(frc);
    }
    stn::Option<rigid_force&> get_force(const tag::Tag& name) {
        for (rigid_force& frc : forces)
        {
            if (frc.tag == name)
            {
                return frc;
            }
        }
        return stn::None;
    }

    void apply_forces() {
        
       forces = forces.filter([](rigid_force val) {return val.has_ended(); }).into<stn::array>();
        for (rigid_force& frc : forces)
        {
            
             acceleration += frc.force / mass;
             int l = 2;
        }

    }
    // Integrate the acceleration to update velocity and position
    //should definitly clamp
    void integrate() {
        
        float deltaTime = CtxName::ctx.Time->dt;

        velocity += acceleration * deltaTime;
        
        if (isonground||isonceil)
        {
            float zero_set_speed = 7;
            velocity.y *= clamp(1 - deltaTime * zero_set_speed, 0.0, 1.0);
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
        size_t mini_steps = 10;
        for (int j = 0; j < mini_steps; j++)
        {


            for (int i = 0; i < 3; i++)
            {
                v3::Vec3 curr_pos = owner.transform().position;
                v3::Vec3 new_pos = curr_pos + v3::Vec3(velocity[i]/mini_steps, i) * deltaTime;
                double scale_in_dir = owner.getcomponent<Collider>().globalbox().scale[i];
                v3::Vec3 max_dir_rel = Vec3(sign(velocity[i]) * scale_in_dir, i);
                ray dir_ray(owner.transform().position + max_dir_rel, max_dir_rel + new_pos);

                if (1.435<curr_pos.y)
                {
                    if (new_pos.y < 1.435) {
                        int l = 5;
                    }
                }
                  voxtra::WorldRayCollision coll = collision::raycastall(dir_ray, collision::HitQuery(owner));
                if (!coll)
                {
                    owner.transform().position[i] = new_pos[i];
                }
                else
                {

                   v3::Vec3 new_posdiff= (*coll).Hit.intersectionpoint - max_dir_rel-v3::Vec3(1e-4f*sign(velocity[i]), i);
                
                   owner.transform().position = new_posdiff;

                }
            }
        }
       // owner.transform().position += velocity * deltaTime;
        acceleration = Vec3(0, 0, 0);  // Reset acceleration after integration
    }

  
};
struct RigidbodySystem :System {

    void run(gameobject::Ecs* ecs) override {

        int l = 6;
         

        query::View< gameobject::transform_comp, rigidbody> rigids(ecs);
        for (auto [pos,body] : rigids) {
            if (isnan(body->velocity.x))
            {
                int l = 2;
            }
            body->oldvelocity = body->velocity;
            body->calculateonground();
            body->calculateonceil();
            if (isnan(body->velocity.x))
            {
                int l = 2;
            }
            body->apply_forces();
            body->integrate();
            if (isnan(body->velocity.x))
            {
                int l = 2;
            }
         //   pos->transform.position = Coord(0, 0, 0); 
        }

    }

};
