#include "entity.h"
#include "aabb.h"
#include <glm/mat4x4.hpp>
#include "../player/player.h"
#include "../renderer/model.h"
#ifndef wormobj_HPP
#define wormobj_HPP
inline float lerpfloat(float f1, float f2, float lval) {

	return f1 * (1 - lval) + f2 * lval;
}
struct worm:gameobject::component
{
	float magoffset;
	int length;
	worm(int len,float offset){
		magoffset = offset;
		length = len;
	}
	void update() {
		
		Transform& currtransform = owner->getcomponent<model>()[0].transform;
		Vector3 pos = player::goblin->transform.position;
		//currtransform.position = pos;
		currtransform.orient(pos);
		
		//currtransform.position += Vector3(1, 0, 0)*.002f;
		currtransform.position +=currtransform.getnormaldirection()* .01f;
		updateglobals();
	}
	void updateglobals() {
		Transform currtransform= owner->getcomponent<model>()[0].transform;
		float speed = .01;
		float lerpv = .01;
	
		for (int i =1; i < length; i++)
		{

			meshname::mesh* meshatpos= owner->getcomponent<model>().meshlist[i];
			aabb::Collider* colatpos = owner->getcomponent<aabb::Collider>(i);
			Transform& local = meshatpos->transform;
			local.position += currtransform.getnormaldirection() * speed;

			colatpos->collideroffset = currtransform.position;
			Transform shouldbetransform = currtransform;
          shouldbetransform.position-=currtransform.getnormaldirection()* magoffset;
         local.pitch = lerpfloat(local.pitch, shouldbetransform.pitch, lerpv);
         local.yaw = lerpfloat(local.yaw, shouldbetransform.yaw, lerpv);
		 local.position = lerp(local.position, shouldbetransform.position, lerpv);
		 currtransform = owner->getcomponent<model>()[i].transform;
		}


	}
	
};

#endif