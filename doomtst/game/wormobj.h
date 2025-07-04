#include "entity.h"
#include "aabb.h"
#include <glm/mat4x4.hpp>
#include "../player/player.h"
#include "../renderer/model.h"
#pragma once 
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
		
	
		updateglobals();
	}
	void updateglobals() {
		Transform currtransform= owner.getcomponent<model>()[0].transform;
		float speed = .01;
		float lerpv = .01;
		aabb::Collider& col = owner.getcomponent<aabb::Collider>();
	

			ModelMeshName::ModelMesh* meshatpos= owner.getcomponent<model>().meshlist[i];
			Transform& local = col.transform();
			local.position += currtransform.getnormaldirection() * speed;

			
			Transform shouldbetransform = currtransform;
          shouldbetransform.position-=currtransform.getnormaldirection()* magoffset;
         local.pitch = lerpfloat(local.pitch, shouldbetransform.pitch, lerpv);
         local.yaw = lerpfloat(local.yaw, shouldbetransform.yaw, lerpv);
		 local.position = lerp(local.position, shouldbetransform.position, lerpv);
		 currtransform = owner.getcomponent<model>()[i].transform;
		}

		
	

};

