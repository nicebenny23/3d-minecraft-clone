#include "vector3.h"
#include "dynamicarray.h"
#include <math.h>
#include "random.h"
#ifndef aabb_HPP
#define aabb_HPP
namespace aabb {
	//ging going to incorporate into engine but have giant array with random selection method
	
	struct colrect
	{
		v3::Vector3 center;
		//scale on each side
		bool gridobj;
		v3::Vector3 scale;
		float mass;
		int index;
		colrect() = default;
		void destroy();
		colrect(const v3::Vector3& objcenter, const v3::Vector3& objscale,bool gridobj);
		
		
	};
	
	void initcolrect();
	
	v3::Vector3 collideaabb(colrect p1, colrect p2);
	extern dynamicarray::array<colrect*> colrectlist;
}

#endif // !aabb_HPP
