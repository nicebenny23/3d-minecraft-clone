#include "../math/vector3.h"
#include "../game/ecs/game_object.h"
#include "../game/aabb.h"
#include "../game/time.h"
#include "../game/entity.h"
#include "../game/objecthelper.h"
#include "../math/geometry.h"
#include "../world/voxeltraversal.h"
#include "ecs/query.h"
#include "../util/pipeline.h"
#include "../util/tag.h"
#include "../game/collision.h"
#include "ecs/query.h"
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
        bool val= CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now()<end_time.unwrap();
        return val;
    }
    rigid_force(Vec3 frc, float duration,const tag::Tag& tg):force(frc), end_time(CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now()+duration), tag(tg) {
        
    }
    rigid_force(Vec3 frc, float duration, const std::string& tg) :force(frc), end_time(CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().now() + duration), tag(tg) {

    }
    rigid_force(Vec3 frc, const tag::Tag& tg) :force(frc), end_time(stn::None),tag(tg){

    }
    rigid_force(Vec3 frc, const std::string& tg) :force(frc), end_time(stn::None), tag(tg) {

    }
    rigid_force():end_time(stn::None),force(zerov) {


    }
};
struct rigidbody : ecs::component{
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
        Point3 boxcenter = owner().get_component<ecs::transform_comp>().transform.position - Vec3(0, boundingbox->globalbox().scale.y + .01, 0);
        geo::Box checkbox = geo::Box(boxcenter, Scale3(boundingbox->globalbox().scale.x, .005, boundingbox->globalbox().scale.z*.9f) * .92);
        isonground = (voxtra::Boxcollwithgrid(checkbox ));
    }
    void calculateonceil() {
		Point3 boxcenter = owner().get_component<ecs::transform_comp>().transform.position + Vec3(0, boundingbox->globalbox().scale.y + .01, 0);
        geo::Box checkbox = geo::Box(boxcenter, Scale3(boundingbox->globalbox().scale.x, .005, boundingbox->globalbox().scale.z) * .9);
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
        oldvelocity = zerov;
        boundingbox = &owner().get_component<aabb::Collider>();  
    }


    void add_force(const rigid_force& frc) {
        forces.push(frc);
    }
    stn::Option<rigid_force&> get_force(const tag::Tag& name) {
        for (rigid_force& frc : forces)
        {
            if (frc.tag == name)
            {
                return stn::Option<rigid_force&>(frc);
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
      
        double deltaTime = CtxName::ctx.Ecs->ensure_resource<timename::TimeManager>().dt;
        velocity += acceleration * deltaTime;
       
        if (isonground||isonceil)
        {
            double zero_set_speed = 7;
            velocity.y *= clamp(1 - deltaTime * zero_set_speed, 0.0, 1.0);
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
                v3::Point3 curr_pos = owner().get_component<ecs::transform_comp>().transform.position;
                v3::Point3 new_pos = curr_pos + v3::Vec3(velocity[i]/mini_steps, i) * deltaTime;
                int sgn = sign(velocity[i]);
                double scale_in_dir = owner().get_component<Collider>().globalbox().scale[i];
                v3::Vec3 max_dir_rel = Vec3(sgn * scale_in_dir, i);
                ray dir_ray(owner().get_component<ecs::transform_comp>().transform.position + max_dir_rel, max_dir_rel + new_pos);

                if (1.435<curr_pos.y)
                {
                    if (new_pos.y < 1.435) {
                        int l = 5;
                    }
                }
                  voxtra::WorldRayCollision coll = collision::raycastall(dir_ray, collision::HitQuery(owner()));
                if (!coll)
                {
                    //we can move completly
                    owner().get_component<ecs::transform_comp>().transform.position[i] = new_pos[i];
                }
                else
                {
                   v3::Point3 new_posdiff= coll.unwrap().Hit.intersectionpoint - max_dir_rel-v3::Vec3(1e-4* sgn, i);
                   Coord pos = coll.unwrap().owner().get_component<block>().pos;
                   owner().get_component<ecs::transform_comp>().transform.position = new_posdiff;

                }
            }
        }
       // owner().get_component<ecs::transform_comp>().transform.position += velocity * deltaTime;
        acceleration = Vec3(0, 0, 0);  
    }

  
};
struct RigidbodySystem :ecs::System {

    void run(ecs::Ecs& ecs) override {

        int l = 6;
         

        ecs::View< ecs::transform_comp, rigidbody> rigids(ecs);
        for (auto [pos,body] : rigids) {
            body.oldvelocity = body.velocity;
            body.calculateonground();
            body.calculateonceil();
            body.apply_forces();
            body.integrate();
         //   pos->transform.position = Coord(0, 0, 0); 
        }

    }

};
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<rigidbody> = {
		.priority=-111
	};
}