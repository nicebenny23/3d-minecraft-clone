#ifndef AABB_HPP
#define AABB_HPP

#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../util/dynamicarray.h"
#include "../util/ray.h"
#include "../util/geometry.h"
#include "../util/random.h"

#include "../util/memorypool.h"
#include <cmath>

namespace aabb {

    struct aabbraycolinfo {
        bool collided;
        float dist;
        v3::Vector3 intersectionpoint;

        aabbraycolinfo()
            : collided(false), dist(INFINITY), intersectionpoint(v3::zerov) {
        
        }

        aabbraycolinfo(bool colided, float orgindist, v3::Vector3 pointofintersection)
            : collided(colided), dist(orgindist), intersectionpoint(pointofintersection) {}
    };

    struct Collider : gameobject::component {
        v3::Vector3 collideroffset;
       

        geometry::Box box;
        v3::Vector3 prevpos;
        bool isunmovable=false;
        unsigned short index;
        bool effector;

        Collider() = default;
        Collider(const v3::Vector3& objcenter, const v3::Vector3& objscale, bool appendtolist,bool iseffector=false);
        ~Collider() = default;

        bool pointinbox(v3::Vector3 pos);
        void ondestroy();
        aabbraycolinfo distanceonray(ray fray);
    };

    void initCollider();
    bool aabbboxintersect(geometry::Box p1, Collider& p2);
   v3::Vector3 collideaabb(Collider p1, Collider p2);

    extern ptrmempool<Collider> Colliderlist;
}

#endif // AABB_HPP