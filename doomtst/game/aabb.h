#ifndef AABB_HPP
#define AABB_HPP

#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../util/dynamicarray.h"
#include "../util/ray.h"
#include "../util/geometry.h"
#include "../util/random.h"
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

    struct colrect : gameobject::component {
        v3::Vector3 center;
        v3::Vector3 scale;
        v3::Vector3 prevpos;
        bool hasrigidbody;
        int index;

        colrect() = default;
        colrect(const v3::Vector3& objcenter, const v3::Vector3& objscale, bool appendtolist);
        ~colrect() = default;

        bool pointinbox(v3::Vector3 pos);
        void destroy();
        aabbraycolinfo distanceonray(ray fray);
    };

    void initcolrect();
    bool aabbboxintersect(geometry::Box p1, colrect& p2);
    bool aabbsintersect(colrect& p1, colrect& p2);
    v3::Vector3 collideaabb(colrect p1, colrect p2);

    extern dynamicarray::array<colrect*> colrectlist;
}

#endif // AABB_HPP