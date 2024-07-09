#include "../util/vector3.h"
#include "../game/gameobject.h"
#include "../util/dynamicarray.h"
#include <math.h>
#include "../util/ray.h"
#include <cmath>

#include "../util/random.h"
#ifndef aabb_HPP
#define aabb_HPP
namespace aabb {
	//ging going to incorporate into engine but have giant array with random selection method
	//also now since evertghin will be an object turn into a compnent
	struct aabbraycolinfo
	{
		bool collided;
		float dist;
		v3::Vector3 intersectionpoint;

		

			aabbraycolinfo() {
				collided = false;
				dist = INFINITY;
			
				intersectionpoint = v3::zerov;
			}
			aabbraycolinfo(bool colided,float orgindist,v3::Vector3 pointofintersection){}
	};
struct	colrect :gameobject::component
	{
		v3::Vector3 center;
		//scale on each side
		bool gridobj;
		v3::Vector3 scale;
		bool pointinbox(v3::Vector3 pos);
		int index;
		//for entities;
		v3::Vector3 prevpos;
		colrect() = default;
		void destroy();
		~colrect() = default;
		colrect(const v3::Vector3& objcenter, const v3::Vector3& objscale,bool gridobj);
		aabbraycolinfo distanceonray(ray fray); 
	};

	void initcolrect();
	bool aabbsintersect(colrect &p1, colrect &p2);
	v3::Vector3 collideaabb(colrect p1, colrect p2);
	extern dynamicarray::array<colrect*> colrectlist;
}

#endif // !aabb_HPP
