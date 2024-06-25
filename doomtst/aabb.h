#include "vector3.h"
#include "gameobject.h"
#include "dynamicarray.h"
#include <math.h>
#include "ray.h"
#include <cmath>
#include "random.h"
#ifndef aabb_HPP
#define aabb_HPP
namespace aabb {
	//ging going to incorporate into engine but have giant array with random selection method
	//also now since evertghin will be an object turn into a compnent
	struct aabbraycollision
	{
		v3::Vector3 intersectionpoint;
			float dist;
			bool intersect;
			aabbraycollision() {

				dist = 1111;
				intersect = false;
				intersectionpoint = v3::zerov;
			}
	};
struct	colrect :gameobject::component
	{
		v3::Vector3 center;
		//scale on each side
		bool gridobj;
		v3::Vector3 scale;
		bool pointinbox(v3::Vector3 pos);
		int index;
		colrect() = default;
		void destroy();
		colrect(const v3::Vector3& objcenter, const v3::Vector3& objscale,bool gridobj);
		aabbraycollision distanceonray(ray fray) {
			v3::Vector3 dir = fray.end - fray.start;
			
			//not actually max
			float xval1 = (center.x - scale.x-fray.start.x) / dir.x;
			float xval2 = (center.x + scale.x - fray.start.x) / dir.x;
			float maxxval = fmax(xval1, xval2);
			float minxval = fmin(xval1, xval2);
			float yval1= (center.y - scale.y - fray.start.y) / dir.y;
			float yval2 = (center.y + scale.y - fray.start.y) / dir.y;
			float maxyval = fmax(yval1, yval2);
			float minyval = fmin(yval1, yval2);
			float zval1 = (center.z - scale.z - fray.start.z) / dir.z;
			float zval2= (center.z + scale.z - fray.start.z) / dir.z;
			float maxzval = fmax(zval1, zval2);
			float minzval = fmin(zval1, zval2);
			float actualminval = fmax(fmax(minxval, minyval), minzval);
			float actualmaxval = fmin(fmin(maxxval, maxyval), maxzval);
			aabbraycollision toreturn = aabbraycollision();
			if (actualminval<actualmaxval)
			{
				float closestt = actualminval;
				if (0<actualminval)
				{
					toreturn.intersect = true;
						toreturn.intersectionpoint = dir * closestt + fray.start;
						
						toreturn.dist= v3::distance(fray.start,toreturn.intersectionpoint);
					
					
				}
				
			}
			
			return toreturn;
		}
		
	};
	
	void initcolrect();
	
	v3::Vector3 collideaabb(colrect p1, colrect p2);
	extern dynamicarray::array<colrect*> colrectlist;
}

#endif // !aabb_HPP
